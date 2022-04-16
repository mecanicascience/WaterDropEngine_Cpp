#include "GUIBar.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::gui {
	void GUIBar::renderMenu() {
		WDE_PROFILE_FUNCTION();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 4.0f));

		// Main file menu
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save Scene"))
				WaterDropEngine::get().getSceneManager().saveScene();
			if (ImGui::MenuItem("Load Scene")) {
				WaterDropEngine::get().getSceneManager().loadScenePath();
			}

			ImGui::Dummy(ImVec2(0.0, 2.5));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0, 0.5));

			if (ImGui::MenuItem("Exit"))
				WaterDropEngine::get().getSubject().notify({LogChannel::RENDER, "WINDOW_SHOULD_CLOSE"});

			ImGui::EndMenu();
		}


		// Window menu
		if (ImGui::BeginMenu("Window")) {
			{
				ImGui::Text("Panels");
				ImGui::Dummy(ImVec2(0.0, 2.0));
				ImGui::PushFont(ImGui::GetIO().FontDefault);

				bool gameObjectsList = true;
				ImGui::Checkbox("Scene Components", &gameObjectsList);
				bool propertiesBool = true;
				ImGui::Checkbox("Properties", &propertiesBool);
				ImGui::Checkbox("Resources Editor", &WaterDropEngine::get().getResourceManager().displayResourceGUI());

				ImGui::PopFont();
			}

			ImGui::Dummy(ImVec2(0.0, 2.5));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0, 0.5));

			{
				ImGui::Text("GUI");
				ImGui::Dummy(ImVec2(0.0, 2.0));
				ImGui::PushFont(ImGui::GetIO().FontDefault);
				ImGui::Checkbox("Enable GUI", &_displayGUI);
				ImGui::PopFont();
			}

			ImGui::EndMenu();
		}


		// Help menu
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("See on Github")) {
				// Note : only works in Windows
				system(std::string("start ").append("https://github.com/mecanicascience/WaterDropEngine").c_str());
			}

			ImGui::Dummy(ImVec2(0.0, 2.0));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0, 2.0));

			// About
			ImGui::Text("About");
			ImGui::Dummy(ImVec2(0.0, 0.5));
			ImGui::PushFont(ImGui::GetIO().FontDefault);
			ImGui::Text("WaterDropEngine is a 3D rendering"
						"\nengine mainly designed for"
						"\ncomputer graphics and physics"
						"\nsimulations using Vulkan.");
			ImGui::Dummy(ImVec2(0.0, 0.5));
			ImGui::PopFont();

			if (ImGui::MenuItem("Made by MecanicaScience")) {
				// Note : only works in Windows
				system(std::string("start ").append("https://www.mecanicascience.fr/").c_str());
			}

			ImGui::EndMenu();
		}

		ImGui::PopStyleVar();
		ImGui::PopFont();
	}
}
