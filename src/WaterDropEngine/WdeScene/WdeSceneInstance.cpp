#include "WdeSceneInstance.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	WdeSceneInstance::WdeSceneInstance() {
		// Create panel
		_worldPartitionPanel = std::make_unique<gui::WorldPartitionPanel>();
	}

	void WdeSceneInstance::tick() {
		double chunkSize = Config::CHUNK_SIZE;
		GameObject* cam = getActiveCamera();
		glm::ivec2 cc { 0, 0 };
		if (cam != nullptr) {
			cc.x = std::floor(cam->transform->position.x / chunkSize + 0.5);
			cc.y = std::floor(cam->transform->position.z / chunkSize + 0.5);
		}

		// Update camera current chunk
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::updateCameraChunk()");
			static glm::ivec2 lastCC = cc;
			if (lastCC != cc) {
				std::shared_ptr<GameObject> camGo = nullptr;
				for (auto &go : getChunkSync(lastCC)->getDynamicGameObjects()) {
					if (cam != nullptr && go->getID() == cam->getID()) {
						camGo = go;
						break;
					}
				}
				if (camGo != nullptr) {
					getChunkSync(cc)->addGameObject(camGo);
					getChunkSync(lastCC)->removeGameObject(cam);
					lastCC = cc;
				}
			}
		}


		// Make sure nearest chunk from the camera are loaded
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::loadNearestChunks()");
			int dist = Config::CHUNK_LOADED_DISTANCE;
			for (int i = -dist; i <= dist; i++) {
				for (int j = -dist; j <= dist; j++) {
					if (i*i + j*j <= dist*dist)
						getChunk({cc.x + i, cc.y + j});
				}
			}
		}

		// Remove chunks that need to be removed
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::removeChunks()");
			/*while (!_removingChunks.empty()) {
				glm::vec2 id = _removingChunks.front();

				if (_activeChunks.contains(id))
					_activeChunks.erase(id);
				_removingChunks.pop();
			}*/
		}

		// Tick for chunks and check if empty ones
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::tickForChunks()");
			int dist = Config::CHUNK_LOADED_DISTANCE;
			for (auto& c : _activeChunks) {
				// Tick
				c.second->tick();

				// Remove not-near chunks
				if ((c.first.x - cc.x)*(c.first.x - cc.x) + (c.first.y - cc.y)*(c.first.y - cc.y) > dist*dist)
					removeChunk(c.first);
			}
		}
	}

	void WdeSceneInstance::cleanUp() {
		WDE_PROFILE_FUNCTION();

		// Delete panel
		_worldPartitionPanel.reset();

		// Clear chunks list
		_activeChunks.clear();
		_removingChunks.clear();

		// Remove references
		_selectedGameObjectChunkID = {0, 0};
		_selectedGameObject = nullptr;
		_activeCamera = nullptr;
	}


	void WdeSceneInstance::onNotify(const core::Event& event) {
#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "CreateGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::createGUI()");

			// Create a game objects list tab
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGuiID dockIDRight = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.19f, nullptr, &dockspaceID);
			ImGui::DockBuilderDockWindow("Scene Components", dockIDRight);

			// Create a properties list
			ImGuiID dockIDProperties = ImGui::DockBuilderSplitNode(dockIDRight, ImGuiDir_Down, 0.75f, nullptr, &dockIDRight);
			ImGui::DockBuilderDockWindow("Properties", dockIDProperties);
		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::drawGUI()");

			// Draw GUI for selected chunk
			{
				// TODO Change selected chunk
				auto ch = getChunk(_selectedGameObjectChunkID);
				if (ch != nullptr)
					ch->drawGUI();
			}


			// Display GUI of loaded chunks
			_worldPartitionPanel->renderPanel();
		}
#endif
	}



	// Chunks
	Chunk* WdeSceneInstance::getChunk(glm::ivec2 chunkID) {
		// Chunk found
		if (_activeChunks.contains(chunkID))
			return _activeChunks.at(chunkID).get();

		// Remove from deletion list
		if (_removingChunks.contains(chunkID)) {
			auto ch = _removingChunks.at(chunkID);
			_activeChunks.emplace(chunkID, ch);
			_removingChunks.erase(chunkID);
			return ch.get();
		}

		// Not found, add to loading list // TODO
		auto ch = std::make_shared<Chunk>(this, chunkID);
		_activeChunks.emplace(chunkID, ch);
		return ch.get();
	}

	Chunk* WdeSceneInstance::getChunkSync(glm::ivec2 chunkID) {
		// Chunk found
		if (_activeChunks.contains(chunkID))
			return _activeChunks.at(chunkID).get();

		// Remove from deletion list
		if (_removingChunks.contains(chunkID)) {
			auto ch = _removingChunks.at(chunkID);
			_activeChunks.emplace(chunkID, ch);
			_removingChunks.erase(chunkID);
			return ch.get();
		}

		// Not found, load sync
		auto ch = std::make_shared<Chunk>(this, chunkID);
		_activeChunks.emplace(chunkID, ch);
		return ch.get();
	}

	void WdeSceneInstance::removeChunk(glm::ivec2 chunkID) {
		if (!_activeChunks.contains(chunkID))
			return;
		_removingChunks.emplace(chunkID, _activeChunks.at(chunkID));
		_activeChunks.erase(chunkID);
	}



	void WdeSceneInstance::reassignGOToChunks() {
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Reassigning game objects to nearest chunks" << logger::endl;

		// Store each go pointer into a vector
		std::vector<std::shared_ptr<GameObject>> _gameObjectsTmp {};
		for (auto& c : _activeChunks) {
			for (auto &go: c.second->getGameObjects())
				_gameObjectsTmp.push_back(go);

			// Clear game objects
			c.second->clearGameObjects();
		}

		// Reassign game objects
		double chunkSize = Config::CHUNK_SIZE;
		for (auto& go : _gameObjectsTmp) {
			// GO chunk position
			glm::ivec2 chunkCoord {
				std::floor(go->transform->position.x / chunkSize + 0.5),
				std::floor(go->transform->position.z / chunkSize + 0.5)
			};

			// Assign
			getChunkSync(chunkCoord)->addGameObject(go);
		}
	}
}
