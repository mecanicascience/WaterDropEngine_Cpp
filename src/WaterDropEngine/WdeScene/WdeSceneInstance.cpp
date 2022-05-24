#include "WdeSceneInstance.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	WdeSceneInstance::WdeSceneInstance() {
		// Create panel
		_worldPartitionPanel = std::make_unique<gui::WorldPartitionPanel>();

		// Create default global set
		{
			// Buffers
			_cameraData = std::make_unique<render::Buffer>(sizeof(Chunk::GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			_objectsData = std::make_unique<render::Buffer>(sizeof(scene::GameObject::GPUGameObjectData) * Config::MAX_SCENE_OBJECTS_COUNT,
																VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

			// Create global descriptor set
			render::DescriptorBuilder::begin()
					.bind_buffer(0, *_cameraData, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.bind_buffer(1, *_objectsData, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(_globalSetDefault.first, _globalSetDefault.second);
		}
	}

	void WdeSceneInstance::tick() {
		// Load and unload chunks
		manageChunks();

		// Update
		GameObject* cam = getActiveCamera();
		glm::ivec2 cc = getCurrentChunkID();

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
						getChunkSync({cc.x + i, cc.y + j});
				}
			}
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

	void WdeSceneInstance::drawGizmo(Gizmo &gizmo, render::CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();

#ifdef WDE_ENGINE_MODE_DEBUG
		// Current Chunk
		double chunkSize = Config::CHUNK_SIZE;
		glm::ivec2 cc = getCurrentChunkID();

		// Draw grid
		int linesDensity = 30;
		auto lm = gizmo.linesManager(Color::GREY);
		for (int i = -6; i <= linesDensity; i++) {
			double shiftX = double(i) / double(linesDensity) * chunkSize - chunkSize / 2.0 + cc.x * chunkSize;
			double shiftY = double(i) / double(linesDensity) * chunkSize - chunkSize / 2.0 + cc.y * chunkSize;

			lm->addLine({shiftX, 0, -chunkSize / 2.0 + cc.y * chunkSize}, {shiftX, 0, chunkSize / 2.0 + cc.y * chunkSize});
			lm->addLine({-chunkSize / 2.0 + cc.x * chunkSize, 0, shiftY}, {chunkSize / 2.0 + cc.x * chunkSize, 0, shiftY});
		}
		lm->drawLines(commandBuffer);
#endif
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

			// Draw GUI for selected chunk (= chunk of current camera)
			{
				_selectedGameObjectChunkID = getCurrentChunkID();
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

		// Not found, add to loading list
		if (std::find(_loadingChunks.begin(), _loadingChunks.end(), chunkID) == _loadingChunks.end())
			_loadingChunks.push_back(chunkID);
		return nullptr;
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
		if (_removingChunks.contains(chunkID) || !_activeChunks.contains(chunkID))
			return;
		_removingChunks.emplace(chunkID, _activeChunks.at(chunkID));
		_activeChunks.erase(chunkID);
	}



	void WdeSceneInstance::manageChunks() {
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::loadChunks()");
			while (!_loadingChunks.empty()) {
				glm::vec2 id = _loadingChunks.back();
				if (_activeChunks.contains(id)) { // Already loaded
					_loadingChunks.pop_back();
					continue;
				}
				if (_removingChunks.contains(id)) // Should be removed but also created => should be created
					_removingChunks.erase(id);

				auto ch = std::make_shared<Chunk>(this, id);
				_activeChunks.emplace(id, ch);
				_loadingChunks.pop_back();
			}
		}

		// Remove chunks that need to be removed
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::removeChunks()");
			auto itr = _removingChunks.begin();
			while (itr != _removingChunks.end()) {
				auto id = itr->first;
				// Chunk is loading
				if (std::find(_loadingChunks.begin(), _loadingChunks.end(), id) == _loadingChunks.end()) {
					itr = _removingChunks.erase(itr);
					continue;
				}

				// Remove from active chunks
				if (_activeChunks.contains(id))
					_activeChunks.erase(id);

				// Remove from removing chunks
				itr = _removingChunks.erase(itr);
			}
		}
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
