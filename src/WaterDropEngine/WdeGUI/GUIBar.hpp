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
			}
	};
}

