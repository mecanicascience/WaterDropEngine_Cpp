#include "WdeSceneInstance.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	void WdeSceneInstance::tick() {
		WDE_PROFILE_FUNCTION();

		// Update game objects
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Ticking for scene dynamic game objects." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::dynamicGameObjects");
			for (auto &go: _gameObjectsDynamic)
				go->tick();
		}

		// Update scene
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Updating scene instance." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::updateScene");
			update();
		}
	}

	void WdeSceneInstance::onNotify(core::Event event) {
#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "CreateGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::createGUI");
			// Create a game objects list tab
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGuiID dockIDLeft = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.19f, nullptr, &dockspaceID);
			ImGui::DockBuilderDockWindow("Scene Components", dockIDLeft);

			// Create a properties list
			ImGuiID dockIDProperties = ImGui::DockBuilderSplitNode(dockIDLeft, ImGuiDir_Down, 0.75f, nullptr, &dockIDLeft);
			ImGui::DockBuilderDockWindow("Properties", dockIDProperties);
		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::drawGUI");
			if (_gameObjects.empty())
				return;

			// Setup scene components list
			ImGui::Begin("Scene Components");
			ImGui::BeginChild("Scene Components Children");
			ImGui::Dummy(ImVec2(0.0f, 0.15f));

			// Draw GUI
			static int selected = -1;
			for (auto& go : _gameObjects)
				if (go->transform->getParent() == nullptr)
					drawGUIForGO(*go, &selected, "");

			// Selected game object changed
			if (selected != _selectedGameObjectID) {
				if (_selectedGameObjectID >= 0)
					_gameObjects[_selectedGameObjectID]->setSelected(false);
				_selectedGameObjectID = selected;
				if (_selectedGameObjectID >= 0)
					_gameObjects[_selectedGameObjectID]->setSelected(true);
			}

			ImGui::EndChild();
			ImGui::End();

			// Render selected game object properties in properties component
			ImGui::Begin("Properties");
			ImGui::Dummy(ImVec2(0.0f, 0.15f));
			if (_selectedGameObjectID != -1)
				_gameObjects.at(_selectedGameObjectID)->drawGUI();
			ImGui::End();
		}
#endif
	}

	void WdeSceneInstance::drawGUIForGO(GameObject& go, int* selected, const std::string& depthIterator) {
		std::string name = std::to_string(go.getID()) + " - " + go.name;

		// Draw element
		ImGui::Dummy(ImVec2(0.2f, 0.0f));
		ImGui::SameLine();
		if (ImGui::Selectable((depthIterator + std::to_string(go.getID()) + " - " + go.name).c_str(), *selected == go.getID()))
			*selected = static_cast<int>(go.getID());

		// Draw children
		if (!go.transform->getChildrenIDs().empty())
			for (int childID : go.transform->getChildrenIDs())
				drawGUIForGO(*_gameObjects[childID], selected, depthIterator + "\t");
	}
}
