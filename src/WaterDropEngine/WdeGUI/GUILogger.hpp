#pragma once

#include "../../wde.hpp"

namespace wde::gui {
	class GUILogger {
		public:
			explicit GUILogger() = default;


			/**
			 * Setups the logger GUI
			 * @param parentID
			 */
			void setupGUI(ImGuiID& parentID) {
				// Create the console tab
				ImGuiID dockIDBottom = ImGui::DockBuilderSplitNode(parentID, ImGuiDir_Down, 0.4f, nullptr, &parentID);
				ImGui::DockBuilderDockWindow("Console", dockIDBottom);
			}

			/**
			 * Renders the last logs to the GUI
			 */
			void renderGUI() {
				// Get messages
				ImGui::Begin("Console");

				// Setup activated channels (as toggle buttons)
				int i = 0;
				auto transparent = ImVec4(0, 0, 0, 0);
				ImGui::PushID(i++);
				ImGui::PushStyleColor(ImGuiCol_Button, transparent);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, transparent);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, transparent);
				ImGui::Button("Activated channels. ");
				ImGui::PopStyleColor(3);
				ImGui::PopID();

				for (auto& [channel, color] : Logger::getChannelColors()) {
					ImGui::SameLine();
					ImGui::PushID(("channels_" + std::to_string(i++)).c_str());

					// Toggle buttons
					auto& activatedChannels = Logger::getActivatedChannels();
					auto containsChannelPosition = std::find(activatedChannels.begin(), activatedChannels.end(), channel);
					if (containsChannelPosition != activatedChannels.end()) { // channel enabled
						ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) color);
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(10, 10, 10, 255));
					}
					else { // channel disabled
						ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(120, 120, 120, 255));
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
					}
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor(color.Value.x * 0.8f, color.Value.y * 0.8f, color.Value.z * 0.8f, color.Value.w));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) color);

					// Toggle channel activation
					if(ImGui::Button(Logger::getNameOf(channel).c_str())) {
						if (containsChannelPosition != activatedChannels.end())
							activatedChannels.erase(containsChannelPosition);
						else
							activatedChannels.push_back(channel);
						Logger::clearMessagesCache();
					}
					ImGui::PopStyleColor(4);
					ImGui::PopID();
				}


				// Select log level
				ImGui::PushID("loglevel_0");
				ImGui::PushStyleColor(ImGuiCol_Button, transparent);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, transparent);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, transparent);
				ImGui::Button("Log level.");
				ImGui::PopStyleColor(3);
				ImGui::PopID();

				static int selectedLogLevel = Logger::getLogLevel();
				static int lastLogLevel = selectedLogLevel;
				ImGui::SameLine();
				ImGui::PushID("loglevel_1");
				ImGui::Combo("", &selectedLogLevel, "ERROR\0WARNING\0INFO\0DEBUG\0");
				ImGui::PopID();
				Logger::setLogLevel(selectedLogLevel);
				ImGui::Separator();
				if (selectedLogLevel != lastLogLevel) {
					lastLogLevel = selectedLogLevel;
					Logger::clearMessagesCache();
				}


				// Display logs
				std::vector<LogMessage>& messages = Logger::getLogLines();
				if (messages.empty()) {
					ImGui::End();
					return;
				}
				for (auto& message : messages) {
					// Displays time
					std::string formattedTime(30, '\0');
					std::strftime(&formattedTime[0], formattedTime.size(), "[%H:%M:%S]", std::localtime(&message.getTime()));
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 180));
					ImGui::Text(formattedTime.c_str());
					ImGui::PopStyleColor();
					ImGui::SameLine();

					// Displays message type
					std::string messageTypeName;
					if (message.getType() == "info") {
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
						messageTypeName = "INFO";
					}
					else if (message.getType() == "error") {
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 72, 255));
						messageTypeName = "ERROR";
					}
					else if (message.getType() == "warning") {
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 181, 72, 255));
						messageTypeName = "WARNING";
					}
					else { // debug
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(210, 210, 210, 255));
						messageTypeName = "DEBUG";
					}
					ImGui::Text(("[" + messageTypeName + "]").c_str());
					ImGui::PopStyleColor();
					ImGui::SameLine();

					// Displays WDE widget
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(136, 230, 255, 180));
					ImGui::Text("[WDE]");
					ImGui::PopStyleColor();
					ImGui::SameLine();

					// Displays channel widget
					ImU32 col = Logger::getChannelColors().at(message.getChannel());
					ImGui::PushStyleColor(ImGuiCol_Text, col);
					ImGui::Text(("[" + Logger::getNameOf(message.getChannel()) + "]").c_str());
					ImGui::PopStyleColor();
					ImGui::SameLine();

					// Set message color depending on the type of the message
					if (message.getType() == "info")
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
					else if (message.getType() == "error")
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 72, 255));
					else if (message.getType() == "warning")
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 181, 72, 255));
					else // debug
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(210, 210, 210, 255));

					// Displays message
					ImGui::Text(message.getMessage().c_str());
					ImGui::PopStyleColor();
				}
				ImGui::End();
			}
	};
}

