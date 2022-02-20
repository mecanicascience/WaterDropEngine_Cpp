#include "GUIBar.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::gui {
	void GUIBar::updateGUI() {

	}

	void GUIBar::renderGUI() {
		// Main file menu
		if (ImGui::BeginMenu("File")) {
			// Closing window menu
			if (ImGui::MenuItem("Close"))
				WaterDropEngine::get().getSubject().notify({LogChannel::RENDER, "WINDOW_SHOULD_CLOSE"});
			// End of menu
			ImGui::EndMenu();
		}


		// GUI menu
		if (ImGui::BeginMenu("GUI")) {
			// Switch gui open and close
			ImGui::Checkbox("Display GUI elements", &_displayGUI);
			// End of menu
			ImGui::EndMenu();
		}
	}
}
