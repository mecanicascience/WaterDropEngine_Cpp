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
				ImGuiID dockIDBottom = ImGui::DockBuilderSplitNode(parentID, ImGuiDir_Down, 0.20f, nullptr, &parentID);
				ImGui::DockBuilderDockWindow("Console", dockIDBottom);
			}

			/**
			 * Renders the last logs to the GUI
			 */
			void renderGUI() {
				// Get messages
				std::vector<LogMessage>& messages = Logger::getLogLines();
				if (messages.empty())
					return;

				ImGui::Begin("Console");
				for (auto& message : messages) {
					// Displays time
					std::string formattedTime(30, '\0');
					std::strftime(&formattedTime[0], formattedTime.size(), "[%H:%M:%S]", std::localtime(&message.getTime()));
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 180));
					ImGui::Text(formattedTime.c_str());
					ImGui::PopStyleColor();
					ImGui::SameLine();

					// Displays message type
					std::string messageTypeName = "";
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

