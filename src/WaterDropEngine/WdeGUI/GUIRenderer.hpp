#pragma once

#include "../../wde.hpp"

namespace wde::gui {
	class GUIRenderer {
		public:
			/**
			 * Create a button describing a 3D Vector
			 * @param label The label of the button
			 * @param values The button X,Y,Z values
			 * @param resetValue The value that will be used to reset the values if the button is clicked
			 * @param columnWidth The width of the columns
			 */
			static void createVec3Button(const std::string& label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 80.0f) {
				ImGui::PushID(label.c_str()); // label vector id
				ImGui::Columns(2); // two columns (one label / one vector)
				ImGui::SetColumnWidth(0, columnWidth);

				// Set label
				ImGui::Text(label.c_str());
				ImGui::NextColumn();


				// Set vector
				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

				// X
				ImGui::PushStyleColor(ImGuiCol_Button, gui::GUITheme::colorRedMajor);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gui::GUITheme::colorRedMinor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, gui::GUITheme::colorRedMajor);
				if(ImGui::Button("X"))
					values.x = resetValue;
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%0.2f");
				ImGui::SameLine();

				// Y
				ImGui::PushStyleColor(ImGuiCol_Button, gui::GUITheme::colorGreenMajor);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gui::GUITheme::colorGreenMinor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, gui::GUITheme::colorGreenMajor);
				if(ImGui::Button("Y"))
					values.y = resetValue;
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%0.2f");
				ImGui::SameLine();

				// Z
				ImGui::PushStyleColor(ImGuiCol_Button, gui::GUITheme::colorBlueMajor);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gui::GUITheme::colorBlueMinor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, gui::GUITheme::colorBlueMajor);
				if(ImGui::Button("Z"))
					values.z = resetValue;
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%0.2f");


				// End vector
				ImGui::PopItemWidth();
				ImGui::PopStyleVar();


				// End of array
				ImGui::PopID();
				ImGui::Columns(1); // empty column
		}
	};
}

