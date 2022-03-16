#include "GameObject.hpp"

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
		if (_isStatic) {
			ImGui::Text("%s", "This game object is static.");
			ImGui::Dummy(ImVec2(0.0f, 3.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		}

		// Render module top
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::Text("%s", (std::to_string(_id) + " - " + name).c_str());
		ImGui::PopFont();
		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

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
