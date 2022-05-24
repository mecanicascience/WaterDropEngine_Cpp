#include "ResourcesPanel.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::gui {
	void ResourcesPanel::render() {
#ifdef WDE_GUI_ENABLED
		WDE_PROFILE_FUNCTION();

		// Draw resources
		ImGui::Begin("Resources Editor");
		bool lastOneOpen = false;
		std::hash<std::string> hasher;

		auto& resByType = WaterDropEngine::get().getResourceManager().getResourcesByType();
		for (auto& resT : resByType) {
			// Small padding between resources
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Header, gui::GUITheme::colorMajorLight);

			// Small padding between modules
			if (lastOneOpen)
				ImGui::Dummy(ImVec2(0.0f, 16.0f));

			// For each resource type
			if (ImGui::CollapsingHeader((resource::Resource::getIcon(resT.first) + "   " + resource::Resource::getName(resT.first)).c_str())) {
				lastOneOpen = true;

				// Render
				ImGui::PopStyleVar(2);
				ImGui::Dummy(ImVec2(0.0f, 1.0f));
				ImGui::PopFont();
				ImGui::PopStyleColor();

				// For each resource
				bool lastOneOpenRes = false;
				for (auto &res: resT.second) {
					// Small padding between resources
					if (lastOneOpenRes)
						ImGui::Dummy(ImVec2(0.0f, 10.0f));

					ImGui::PushID(static_cast<int>(hasher(res.second->getPath()) + 1581542));
					ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
					if (ImGui::TreeNode(res.second->getName().c_str())) {
						ImGui::PopFont();
						lastOneOpenRes = true;
						res.second->drawGUI();
						ImGui::TreePop();
					}
					else {
						ImGui::PopFont();
						lastOneOpenRes = false;
					}
					ImGui::PopID();
				}
			}
			else {
				lastOneOpen = false;

				ImGui::PopStyleVar(2);
				ImGui::PopFont();
				ImGui::PopStyleColor();
			}
		}
		ImGui::End();
#endif
	}
}
