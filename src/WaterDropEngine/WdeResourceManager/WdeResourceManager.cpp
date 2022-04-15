#include "WdeResourceManager.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::resource {
	// Core methods
	WdeResourceManager::WdeResourceManager(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) { }

	void WdeResourceManager::tick() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RES) << "Ticking for resource manager." << logger::endl;

		// Delete resources that need to be deleted
		for (auto& res : _resourcesToDelete) {
			// Decrease ticks
			res.second.first--;

			// If no ticks remaining, delete
			if (res.second.first <= 0) {
				if (res.second.second->getReferenceCount() > 0) {
					_resourcesToDelete.erase(res.first);
					continue;
				}

				logger::log(LogLevel::DEBUG, LogChannel::RES) << "Releasing resource \"" << res.first << "\"." << logger::endl;
				_resourcesByType[res.second.second->getType()].erase(res.first);
				_resources.erase(res.first);
				_resourcesToDelete.erase(res.first);
			}
		}
	}

	void WdeResourceManager::cleanUp() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RES) << "== Cleaning Up Resource Manager ==" << logger::endl;

		// Release every lasting resources
		_resourcesByType.clear();
		for (auto& res : _resources)
			res.second.reset();
		_resources.clear();

		logger::log(LogLevel::DEBUG, LogChannel::RES) << "== Cleaning Up Done ==" << logger::endl;
	}

	void WdeResourceManager::onNotify(const core::Event& event) {
#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "CreateGUI") {
			// Create a game objects list tab
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGui::DockBuilderDockWindow("Resources Editor", dockspaceID);
		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI" && _displayResourceGUI) {
			// Draw resources
			ImGui::Begin("Resources Editor");
			bool lastOneOpen = false;
			std::hash<std::string> hasher;

			for (auto& resT : _resourcesByType) {
				// Small padding between resources
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_Header, gui::GUITheme::colorMajorLight);

				// Small padding between modules
				if (lastOneOpen)
					ImGui::Dummy(ImVec2(0.0f, 16.0f));

				// For each resource type
				if (ImGui::CollapsingHeader((Resource::getIcon(resT.first) + "   " + Resource::getName(resT.first)).c_str())) {
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
		}
#endif
	}
}
