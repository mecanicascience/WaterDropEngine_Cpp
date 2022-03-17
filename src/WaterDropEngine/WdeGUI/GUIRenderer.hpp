#pragma once

#include "../../wde.hpp"
#include "GUITheme.hpp"

namespace wde::gui {
	class GUIRenderer : public NonCopyable {
		public:
			// Windows rendering
			/** Push the window tab size style */
			static void pushWindowTabStyle() {
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorWhiteMajor);
			}

			/** Pop the window tab size style */
			static void popWindowTabStyle() {
				ImGui::PopStyleColor();
				ImGui::PopFont();
			}


			// Drawing components
			/**
			 * Draws text horizontally centered. Cf https://stackoverflow.com/questions/64653747/how-to-center-align-text-horizontally
			 * @param text
			 */
			static void textCentered(const std::string& text) {
				float win_width = ImGui::GetWindowSize().x;
				float text_width = ImGui::CalcTextSize(text.c_str()).x;

				// calculate the indentation that centers the text on one line, relative
				// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
				float text_indentation = (win_width - text_width) * 0.5f;

				// if text is too long to be drawn on one line, `text_indentation` can
				// become too small or even negative, so we check a minimum indentation
				float min_indentation = 20.0f;
				if (text_indentation <= min_indentation) {
					text_indentation = min_indentation;
				}

				ImGui::SameLine(text_indentation);
				ImGui::PushTextWrapPos(win_width - text_indentation);
				ImGui::TextWrapped("%s", text.c_str());
				ImGui::PopTextWrapPos();
			}


			/**
			 * Create a button describing a 3D Vector
			 * @param label The label of the button
			 * @param values The button X,Y,Z values
			 * @param resetValue The value that will be used to reset the values if the button is clicked (default 0)
			 * @param columnWidth The width of the columns (default 80)
			 */
			static void addVec3Button(const std::string& label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 80.0f) {
				ImGui::PushID(label.c_str()); // label vector id
				ImGui::Columns(2); // two columns (one label / one vector)
				ImGui::SetColumnWidth(0, columnWidth);

				// Set label
				ImGui::Text("%s", label.c_str());
				ImGui::NextColumn();

				// Set vector
				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

				GUIRenderer::addFloatDraggerButton("X", values.x, resetValue, gui::GUITheme::colorRedMajor, gui::GUITheme::colorRedMinor); // X
				ImGui::SameLine();
				GUIRenderer::addFloatDraggerButton("Y", values.y, resetValue, gui::GUITheme::colorGreenMajor, gui::GUITheme::colorGreenMinor); // Y
				ImGui::SameLine();
				GUIRenderer::addFloatDraggerButton("Z", values.z, resetValue, gui::GUITheme::colorBlueMajor, gui::GUITheme::colorBlueMinor); // Z

				ImGui::PopItemWidth();
				ImGui::PopStyleVar();

				// End of array
				ImGui::PopID();
				ImGui::Columns(1); // empty column
			}


			/**
			 * Add a float drager with a label
			 * @param label The label next to the button
			 * @param value A reference to the dragger value
			 * @param resetValue The reset value (default 0)
			 * @param columnWidth The label column width (default 120)
			 */
			static void addFloatDragger(const std::string& label, float &value, float resetValue = 0.0f, float columnWidth = 120.0f, float cursorSpeed = 0.1f) {
				// Array
				ImGui::PushID(label.c_str());
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, columnWidth);

				// Label
				ImGui::Text("%s", label.c_str());
				ImGui::NextColumn();

				// Drag
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
				addFloatDraggerButton("Reset", value, resetValue, GUITheme::colorMajorLight, GUITheme::colorMinorLight, cursorSpeed);
				ImGui::PopStyleVar();

				// End of array
				ImGui::PopID();
				ImGui::Columns(1);
			}



		private:
			/**
			 * Creates a float dragger
			 * @param name Label
			 * @param value A reference to the value
			 * @param resetValue The reset value when the user clicks on the associated button (default 0)
			 * @param buttonColor Color of button selected and active (default colorMajorLight)
			 * @param hoverColor Color of button hovered (default colorMinorLight)
			 */
			static void addFloatDraggerButton(const std::string& name, float &value, float resetValue = 0.0f, ImVec4 buttonColor = GUITheme::colorMajorLight, ImVec4 hoverColor = GUITheme::colorMinorLight, float cursorSpeed = 0.1f) {
				ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
				if(ImGui::Button(name.c_str()))
					value = resetValue;
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat(("##" + name).c_str(), &value, cursorSpeed, 0.0f, 0.0f, "%0.2f");
			}
	};
}
