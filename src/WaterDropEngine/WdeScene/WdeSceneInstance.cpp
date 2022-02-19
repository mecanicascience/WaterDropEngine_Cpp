#include "WdeSceneInstance.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	void WdeSceneInstance::tick() {
		WDE_PROFILE_FUNCTION();

		// Update game objects
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Ticking for scene game objects." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tickGameObjects");
			for (auto &go: _gameObjects)
				go->tick();
		}

		// Update scene
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Updating scene instance." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::updateScene");
			update();
		}
	}

	void WdeSceneInstance::onNotify(core::Event event) {
		if (event.channel == LogChannel::GUI && event.name == "CreateGUI") {
			WDE_PROFILE_FUNCTION();
			// Create a game objects list tab
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGuiID dockIDLeft = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.19f, nullptr, &dockspaceID);
			ImGui::DockBuilderDockWindow("Scene Components", dockIDLeft);

			// Create a properties list
			ImGuiID dockIDProperties = ImGui::DockBuilderSplitNode(dockIDLeft, ImGuiDir_Down, 0.75f, nullptr, &dockIDLeft);
			ImGui::DockBuilderDockWindow("Properties", dockIDProperties);
		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			WDE_PROFILE_FUNCTION();
			// Setup scene components list
			ImGui::Begin("Scene Components");
			ImGui::BeginChild("Scene Components Children");
			ImGui::Dummy(ImVec2(0.0f, 0.15f));
			static int selected = -1;
			for (auto& go : _gameObjects)
				if (go->transform->getParent() == nullptr)
					drawGUIForGO(*go, &selected, "");
			_selectedGameObjectID = selected;
			ImGui::EndChild();
			ImGui::End();

			// Render selected game object properties in properties component
			ImGui::Begin("Properties");
			ImGui::Dummy(ImVec2(0.0f, 0.15f));
			if (_selectedGameObjectID != -1)
				_gameObjects.at(_selectedGameObjectID)->drawGUI();
			ImGui::End();
		}
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
