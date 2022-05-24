#include "GameObject.hpp"
#include "modules/MeshRendererModule.hpp"
#include "modules/CameraModule.hpp"
#include "../WaterDropEngine.hpp"

/**
 * Add Combo with std vector methods to ImGUI.
 */
namespace ImGui {
	static auto vector_getter = [](void* vec, int idx, const char** out_text) {
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter, static_cast<void*>(&values), static_cast<int>(values.size()));
	}
}

namespace wde::scene {
	GameObject::GameObject(uint32_t id, std::string name, bool isStatic) : _id(id), name(std::move(name)), _isStatic(isStatic) {
		WDE_PROFILE_FUNCTION();
		// Add default transform module
		transform = addModule<TransformModule>();
	}

	GameObject::~GameObject() {
		WDE_PROFILE_FUNCTION();
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
#ifdef WDE_GUI_ENABLED
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
		auto textS = ImGui::CalcTextSize("     ");
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::PushID(static_cast<int>(_id) + 2312951);
			if (active)
				ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorTextMinor);
			else
				ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);

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
		}

		// Display object name
		{
			if (ImGui::BeginPopupContextItem("edit_object_name")) {
				// Edit
				static char nameLoc[128] = "";
				memset(nameLoc, 0, IM_ARRAYSIZE(nameLoc));
				for (int i = 0; i < IM_ARRAYSIZE(nameLoc); i++) {
					if (i < name.size())
						nameLoc[i] = name[i];
				}
				ImGui::Text("Edit name :");
				ImGui::InputText("##edit", nameLoc, IM_ARRAYSIZE(nameLoc));
				ImGui::Separator();
				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				name = nameLoc;

				// Delete object
				ImGui::SameLine();
				ImGui::Dummy({2.0f, 0.0f});
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, gui::GUITheme::colorRedMajor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, gui::GUITheme::colorRedMinor);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gui::GUITheme::colorRedMinor);
				if (ImGui::Button("Delete")) {
					auto scene = WaterDropEngine::get().getInstance().getScene();
					auto ch = scene->getChunk(scene->getSelectedGameObjectChunk());
					if (ch != nullptr)
						ch->removeGameObject(this);
					ImGui::CloseCurrentPopup();
				}
				ImGui::PopStyleColor(3);
				ImGui::EndPopup();
			}

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
			ImGui::OpenPopupOnItemClick("edit_object_name", ImGuiPopupFlags_MouseButtonRight);
			ImGui::PopFont();
		}

		// Left camera icon
		{
			auto cameraMod = getModule<CameraModule>();
			if (cameraMod != nullptr) {
				// Icon
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
				auto scene = WaterDropEngine::get().getInstance().getScene();
				ImGui::PushID(static_cast<int>(_id) + 6496321);
				if (scene->getActiveCamera() != nullptr && scene->getActiveCamera()->getID() == _id) { // Current selected camera
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
				if (scene->getActiveCamera() != nullptr && scene->getActiveCamera()->getID() == _id)
					ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorTextMinor);
				else
					ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
				ImGui::Text("Select");
				ImGui::PopStyleColor();
				ImGui::PopFont();
				ImGui::SameLine();
			}
		}

		// Object type
		{
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
		}

		// Render Modules GUI
		{
			bool lastOneOpen = false;
			for(auto& module : _modules) {
				if (module == nullptr)
					continue;

				// Small padding between modules
				if (lastOneOpen)
					ImGui::Dummy(ImVec2(0.0f, 14.0f));

				// Render module
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_Header, gui::GUITheme::colorMajorLight);

				std::string dispStr;
				if (!module->getIcon().empty())
					dispStr = module->getIcon().append("    ");
				if (ImGui::CollapsingHeader((dispStr + module->getName()).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
					bool moduleRemoved = false;
					if (module->getName() != "Transform" && ImGui::BeginPopupContextItem()) {
						if (ImGui::Button("Remove Module")) {
							if (module->getName() == "Camera" && WaterDropEngine::get().getInstance().getScene()->getActiveCamera() == this)
								WaterDropEngine::get().getInstance().getScene()->setActiveCamera(nullptr);
							moduleRemoved = true;
							ModuleSerializer::removeModuleFromName(module->getName(), *this);
						}
						ImGui::EndPopup();
					}
					lastOneOpen = true;
					ImGui::PopStyleVar(2);
					ImGui::Dummy(ImVec2(0.0f, 1.0f));
					ImGui::PopFont();
					ImGui::PopStyleColor();

					// Render header content
					ImGui::PushFont(ImGui::GetIO().FontDefault);
					if (!moduleRemoved)
						module->drawGUI();
					ImGui::PopFont();
				}
				else {
					if (module->getName() != "Transform" && ImGui::BeginPopupContextItem()) {
						if (ImGui::Button("Remove Module"))
							ModuleSerializer::removeModuleFromName(module->getName(), *this);
						ImGui::EndPopup();
					}
					lastOneOpen = false;
					ImGui::PopStyleVar(2);
					ImGui::PopFont();
					ImGui::PopStyleColor();
				}
			}
		}

		// Add Modules Option
		{
			auto modules = ModuleSerializer::getRemainingModulesList(*this);
			if (!modules.empty()) {
				static int item_current = 0;
				ImGui::Dummy(ImVec2(0.0f, 4.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 2.0f));

				const float width = ImGui::GetWindowWidth();
				ImGui::SetNextItemWidth(width * 0.6f);
				ImGui::Combo("##Select Module", &item_current, modules);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(width * 0.3f);
				if (ImGui::Button("Add Module"))
					ModuleSerializer::addModuleFromName(modules[item_current], "", *this);
			}
		}
#endif
	}
}
