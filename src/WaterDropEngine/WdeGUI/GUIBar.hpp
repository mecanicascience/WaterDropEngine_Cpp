#pragma once

#include "../../wde.hpp"

namespace wde::gui {
	class GUIBar {
		public:
			/**
			 * Renders the gui top menu
			 */
			void renderGUI() {
				// Main file menu
				if (ImGui::BeginMenu("File")) {
					// Closing window menu
					if (ImGui::MenuItem("Close"))
						WdeRenderEngine::get().getCoreWindow().closeWindow();

					// End of menu
					ImGui::EndMenu();
				}


				// GUI file menu
				if (ImGui::BeginMenu("GUI")) {
					// Switch gui open and close
					ImGui::Checkbox("Display GUI elements", &_displayGUI);

					// End of menu
					ImGui::EndMenu();
				}
			}


			// Getters and setters
			bool displayGUI() { return _displayGUI; }


		private:
			/** Display GUI by default */
			bool _displayGUI = true;
	};
}

