#include "GameObject.hpp"
#include "modules/MeshRendererModule.hpp"
#include "modules/CameraModule.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::scene {
	GameObject::GameObject(uint32_t id, std::string name, bool isStatic) : _id(id), name(std::move(name)), _isStatic(isStatic) {
		// Add default transform module
		transform = addModule<TransformModule>();
	}

	GameObject::~GameObject() {
		transform = nullptr;
		_modules.clear();
	}

	void GameObject::tick() {
		if (!active)
			return;

		WDE_PROFILE_FUNCTION();
		// Tick for modules
		for (auto& mod : _modules)
			mod->tick();
	}

	void GameObject::drawGUI() {
		WDE_PROFILE_FUNCTION();

		// Type of the object
		std::string typeName;
		if (getModule<MeshRendererModule>())
			typeName = "Mesh Entity";
		else if (getModule<CameraModule>())
			typeName = "Camera";
		else
			typeName = "Entity";


		// Left view icon
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::PushID(static_cast<int>(_id) + 2312951);
		if (active)
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorTextMinor);
		else
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
		auto textS = ImGui::CalcTextSize("     ");
		if (active && ImGui::Selectable(ICON_FA_EYE, false, 0, textS) || (!active && ImGui::Selectable(ICON_FA_EYE_SLASH, false, 0, textS)))
			active = !active;
		ImGui::SameLine();
		ImGui::PopStyleColor();
		ImGui::PopID();
		if (active)
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorTextMinor);
		else
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
		ImGui::Text("Display");
		ImGui::PopStyleColor();
		ImGui::PopFont();

		// Display object name
		ImGui::SameLine();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		char buf[4 + name.size() + 5];
		if (typeName == "Mesh Entity")
			sprintf(buf, ICON_FA_GHOST "   %s", name.c_str());
		else if (typeName == "Camera")
			sprintf(buf, ICON_FA_CAMERA "   %s", name.c_str());
		else
			sprintf(buf, ICON_FA_FOLDER "   %s", name.c_str());
		gui::GUIRenderer::textCentered(buf);
		ImGui::PopFont();

		// Left camera icon
		auto cameraMod = getModule<CameraModule>();
		if (cameraMod != nullptr) {
			// Icon
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			auto& scene = WaterDropEngine::get().getInstance().getScene();
			ImGui::PushID(static_cast<int>(_id) + 6496321);
			if (scene.getActiveCamera() != nullptr && scene.getActiveCamera()->getID() == _id) { // Current selected camera
				ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorTextMinor);
				if (ImGui::Selectable(ICON_FA_CAMERA, false, 0, textS)) {}
			}
			else { // Camera not selected
				ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
				if (ImGui::Selectable(ICON_FA_CAMERA, false, 0, textS))
					cameraMod->setAsActive();
			}
			ImGui::PopStyleColor();
			ImGui::PopID();

			// Label
			ImGui::SameLine();
			if (scene.getActiveCamera() != nullptr && scene.getActiveCamera()->getID() == _id)
				ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorTextMinor);
			else
				ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			ImGui::Text("Select");
			ImGui::PopStyleColor();
			ImGui::PopFont();
			ImGui::SameLine();
		}

		// Object type
		ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
		ImGui::PushFont(ImGui::GetIO().FontDefault);
		ImGui::Dummy(ImVec2(0.0f, ImGui::CalcTextSize("test").y));
		if (_isStatic)
			gui::GUIRenderer::textCentered("Static " + typeName + "");
		else
			gui::GUIRenderer::textCentered("" + typeName + "");
		ImGui::PopFont();
		ImGui::PopStyleColor();

		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::Separator();


		// Render Modules GUI
		int count = 0;
		for(auto& module : _modules) {
			// Add small space between modules
			if (count != 0)
				ImGui::Dummy(ImVec2(0.0f, 18.0f));
			count++;

			// Render module
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			ImGui::Dummy(ImVec2(6.0f, 0.0f));
			ImGui::SameLine();
			if (ImGui::CollapsingHeader(module->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Dummy(ImVec2(0.0f, 1.0f));
				ImGui::PopFont();
				// Render header content
				module->drawGUI();
			}
			else
				ImGui::PopFont();
		}
	}
}
