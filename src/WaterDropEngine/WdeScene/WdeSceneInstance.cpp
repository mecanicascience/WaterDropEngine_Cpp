#include "WdeSceneInstance.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	void WdeSceneInstance::tick() {
		WDE_PROFILE_FUNCTION();

		// Remove game objects to delete
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Removing deleted game objects." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::deleteGameObjects");
			for (auto& go : _gameObjectsToDelete) {
				/*_selectedGameObjectID = -1;
				 * TODO

				// Remove from static list
				if (go->isStatic()) {
					for (auto& go : _gameObjectsStatic)
					_gameObjectsStatic.r
				}*/
			}
		}

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

	void WdeSceneInstance::cleanUpInstance() {
		_gameObjectsDynamic.clear();
		_gameObjectsStatic.clear();
		_gameObjects.clear();

		// Clean up scene
		cleanUp();
	}


	void WdeSceneInstance::onNotify(const core::Event& event) {
#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "CreateGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::createGUI");
			// Create a game objects list tab
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGuiID dockIDRight = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.19f, nullptr, &dockspaceID);
			ImGui::DockBuilderDockWindow("Scene Components", dockIDRight);

			// Create a properties list
			ImGuiID dockIDProperties = ImGui::DockBuilderSplitNode(dockIDRight, ImGuiDir_Down, 0.75f, nullptr, &dockIDRight);
			ImGui::DockBuilderDockWindow("Properties", dockIDProperties);
		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::drawGUI");
			if (_gameObjects.empty())
				return;

			// Setup scene components list
			gui::GUIRenderer::pushWindowTabStyle();
			ImGui::Begin("Scene Components");
			gui::GUIRenderer::popWindowTabStyle();
			ImGui::BeginChild("Scene Components Children");
			ImGui::Dummy(ImVec2(0.0f, 0.15f));

			// Add Game Object button
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			if (ImGui::Button(ICON_FA_PLUS_CIRCLE))
				createGameObject("Empty Gameobject");
			ImGui::PopFont();
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			ImGui::PushFont(ImGui::GetIO().FontDefault);
			ImGui::SameLine();
			ImGui::Text("Add an empty Gameobject");
			ImGui::PopFont();
			ImGui::PopStyleColor();
			ImGui::Separator();

			// Scene game objects
			ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoClip;
			static int selected = -1;
			if (ImGui::BeginTable("Game Objects List", 3, flags)) {
				// Draw game objects list
				for (auto& go : _gameObjects) {
					if (go->transform->getParent() == nullptr) {
						ImGui::TableNextRow();
						drawGUIForGo(go, &selected);
					}
				}
				ImGui::EndTable();
			}

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
			gui::GUIRenderer::pushWindowTabStyle();
			ImGui::Begin("Properties");
			gui::GUIRenderer::popWindowTabStyle();
			ImGui::PushFont(ImGui::GetIO().FontDefault);
			ImGui::Dummy(ImVec2(0.0f, 0.15f));
			if (_selectedGameObjectID != -1)
				_gameObjects.at(_selectedGameObjectID)->drawGUI();
			ImGui::End();
			ImGui::PopFont();
		}
#endif
	}

	void WdeSceneInstance::drawGUIForGo(const std::shared_ptr<GameObject> &go, int* selected) const {
#ifdef WDE_GUI_ENABLED
		std::string typeName;
		if (go->getModule<MeshRendererModule>())
			typeName = "Mesh Entity";
		else if (go->getModule<CameraModule>())
			typeName = "Camera";

		// Enable / disabled icon
		ImGui::TableSetColumnIndex(0);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		bool notAct = false;
		if (!go->active) {
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			notAct = true;
		}
		ImGui::PushID(static_cast<int>(go->getID()) + 216846351);
		auto textS = ImGui::CalcTextSize("      ");
		if ((go->active && ImGui::Selectable(" " ICON_FA_EYE, false, 0, textS)) || (!go->active && ImGui::Selectable(" " ICON_FA_EYE_SLASH, false, 0, textS)))
			go->active = !go->active;
		ImGui::PopID();
		if (notAct)
			ImGui::PopStyleColor();
		ImGui::PopFont();


		// Content
		ImGui::TableSetColumnIndex(1);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::PushID(static_cast<int>(go->getID()) + 216846352);
		bool hasNode = false;
		if (!go->transform->getChildrenIDs().empty() && ImGui::TreeNode("")) {
			hasNode = true;
			// Compute buffer without offset
			char buf3[4 + go->name.size() + 5];
			if (typeName == "Mesh Entity")
				sprintf(buf3, ICON_FA_GHOST "  %s", go->name.c_str());
			else if (typeName == "Camera")
				sprintf(buf3, ICON_FA_CAMERA "  %s", go->name.c_str());
			else
				sprintf(buf3, ICON_FA_FOLDER_OPEN "  %s", go->name.c_str());

			// Draw tree node
			ImGui::SameLine();
			ImGui::PushID(static_cast<int>(go->getID()) + 216846353);
			if (ImGui::Selectable(buf3, *selected == go->getID(), ImGuiSelectableFlags_SpanAllColumns))
				*selected = static_cast<int>(go->getID());
			ImGui::PopID();
			ImGui::PopFont();

			// Type name
			ImGui::TableSetColumnIndex(2);
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			if (typeName.size() > 3)
				ImGui::Text("%s", ((go->isStatic() ? "Static " : "") + typeName + "   ").c_str());
			ImGui::PopStyleColor();

			// Draw for children
			for (int childID : go->transform->getChildrenIDs()) {
				ImGui::TableNextRow();
				drawGUIForGo(_gameObjects[childID], selected);
			}

			ImGui::TreePop();
		}
		ImGui::PopID();

		if (!hasNode) {
			char buf2[4 + go->name.size() + 5];
			std::string extraSpace;
			if (go->transform->getChildrenIDs().empty())
				extraSpace = "     ";

			if (typeName == "Mesh Entity")
				sprintf(buf2, (extraSpace + " " + ICON_FA_GHOST "   %s").c_str(), go->name.c_str());
			else if (typeName == "Camera")
				sprintf(buf2, (extraSpace + " " + ICON_FA_CAMERA "   %s").c_str(), go->name.c_str());
			else
				sprintf(buf2, (extraSpace + " " + ICON_FA_FOLDER "   %s").c_str(), go->name.c_str());

			ImGui::SameLine();
			ImGui::PushID(static_cast<int>(go->getID()) + 216846354);
			if (ImGui::Selectable(buf2, *selected == go->getID(), ImGuiSelectableFlags_SpanAllColumns))
				*selected = static_cast<int>(go->getID());
			ImGui::PopID();
			ImGui::PopFont();

			// Type name
			ImGui::TableSetColumnIndex(2);
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			if (typeName.size() > 3)
				ImGui::Text("%s", ((go->isStatic() ? "Static " : "") + typeName).c_str());
			ImGui::PopStyleColor();
		}
#endif
	}
}
