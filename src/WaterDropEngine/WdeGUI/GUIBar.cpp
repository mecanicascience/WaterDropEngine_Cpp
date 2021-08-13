#include "GUIBar.hpp"
#include "../WdeScene/WdeSceneManager.hpp"

namespace wde::gui {
	void GUIBar::updateGUI() {
		if (_shouldLoadScene) {
			// Open load scene dialog
			std::vector<char> fileContent = WdeFileUtils::readFileDialog("json");
			if (!fileContent.empty()) {
				std::string strContent (fileContent.begin(), fileContent.end());
				auto sceneJSON = json::parse(strContent);
				scene::WdeSceneManager::get().deserializeScene(sceneJSON);
			}

			_shouldLoadScene = false;
		}
	}

	void GUIBar::renderGUI() {
		// Main file menu
		if (ImGui::BeginMenu("File")) {
			// Closing window menu
			if (ImGui::MenuItem("Close"))
				WdeRenderEngine::get().getCoreWindow().closeWindow();

			// End of menu
			ImGui::EndMenu();
		}

		// Scene menu
		if (ImGui::BeginMenu("Scene")) {
			// Save scene
			if (ImGui::Button("Save scene"))
				scene::WdeSceneManager::get().serializeScene();

			// Load scene
			if (ImGui::Button("Load scene"))
				_shouldLoadScene = true;

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


