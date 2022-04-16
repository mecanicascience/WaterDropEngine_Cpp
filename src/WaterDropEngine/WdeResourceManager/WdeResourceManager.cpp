#include "WdeResourceManager.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::resource {
	// Core methods
	WdeResourceManager::WdeResourceManager(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) { }

	void WdeResourceManager::tick() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RES) << "Ticking for resource manager." << logger::endl;

		// Select resources that need to be deleted
		std::vector<decltype(_resourcesToDelete)::key_type> vecToDelete;
		for (auto&& el : _resourcesToDelete) {
			// Decrease ticks
			el.second--;

			// If no ticks remaining, delete
			if (el.second <= 0) {
				// Resource not loaded or reloaded since last time
				if (!_resources.contains(el.first) || _resources.at(el.first)->getReferenceCount() > 0) {
					vecToDelete.emplace_back(el.first);
					continue;
				}

				// Release resource
				_resourcesByType[_resources.at(el.first)->getType()].erase(el.first);
				_resources.erase(el.first);
				vecToDelete.emplace_back(el.first);
			}
		}

		// Erase resources that need to be deleted
		for (auto&& key : vecToDelete) {
			logger::log(LogLevel::DEBUG, LogChannel::RES) << "Releasing resource \"" << key << "\"." << logger::endl;
			_resourcesToDelete.erase(key);
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
			WDE_PROFILE_SCOPE("wde::resource::WdeResourceManager::WdeResourceManager::createGUI()");
			// Create a game objects list tab
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGui::DockBuilderDockWindow("Resources Editor", dockspaceID);
		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI" && _displayResourceGUI) {
			WDE_PROFILE_SCOPE("wde::resource::WdeResourceManager::WdeResourceManager::drawGUI()");
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
