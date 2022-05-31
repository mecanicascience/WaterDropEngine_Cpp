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
			_objectsData = std::make_unique<render::Buffer>(sizeof(scene::GameObject::GPUGameObjectData) * Config::MAX_CHUNK_OBJECTS_COUNT,
																VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

			// Create global descriptor set
			render::DescriptorBuilder::begin()
					.bind_buffer(0, *_cameraData, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.bind_buffer(1, *_objectsData, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(_globalSetDefault.first, _globalSetDefault.second);
		}
	}

	void WdeSceneInstance::tick() {
		// Create editor camera
#ifdef WDE_ENGINE_MODE_DEBUG
		if (_isFirstTick) {
			_isFirstTick = false;
			_editorCamera = std::make_unique<GameObject>(-1, "Editor Camera", false);
			auto camModule = _editorCamera->addModule<scene::CameraModule>();
			camModule->setAsActive();
			camModule->setFarPlane(std::numeric_limits<float>::max());
			setActiveCamera(_editorCamera.get());
			_editorCamera->addModule<scene::ControllerModule>();
			_editorCamera->transform->position = glm::vec3 {0.0f, 0.0f, 0.0f};
			_editorCamera->transform->rotation = glm::vec3 {0.0f, 0.0f, 0.0f};
		}
#endif

		// Load and unload chunks
		manageChunks();

		// Update
		GameObject* cam = getActiveCamera();
		glm::ivec2 cc = getCurrentChunkID();

		// Update camera current chunk
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::updateCameraChunk()");
			static glm::ivec2 lastCC = cc;
			if (cam != nullptr && lastCC != cc && cam->name != "Editor Camera") {
				for (auto& go : getChunkSync(lastCC)->getGameObjects()) {
					if (go.get() == cam) {
						getChunkSync(cc)->addGameObject(go);
						getChunkSync(lastCC)->removeGameObject(cam);
						break;
					}
				}
			}
			lastCC = cc;

			// Update editor camera
#ifdef WDE_ENGINE_MODE_DEBUG
			_editorCamera->tick();
#endif
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
			auto it = _activeChunks.begin();
			while(it != _activeChunks.end()) {
				// Tick
				it->second->tick();

				// Remove not-near chunks
				if ((it->first.x - cc.x)*(it->first.x - cc.x) + (it->first.y - cc.y)*(it->first.y - cc.y) > dist*dist)
					removeChunk(it->first);

				it++;
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

	void WdeSceneInstance::drawGizmo(Gizmo &gizmo) {
#ifdef WDE_ENGINE_MODE_DEBUG
		WDE_PROFILE_FUNCTION();

		// Current Chunk
		double chunkSize = Config::CHUNK_SIZE;
		glm::ivec2 cc = getCurrentChunkID();

		// Draw grid
		int linesDensity = 30;
		auto lm = gizmo.linesManager(Color::GREY);
		for (int i = 0; i <= linesDensity; i++) {
			double shiftX = double(i) / double(linesDensity) * chunkSize - chunkSize / 2.0 + cc.x * chunkSize;
			double shiftY = double(i) / double(linesDensity) * chunkSize - chunkSize / 2.0 + cc.y * chunkSize;

			lm->addLine({shiftX, 0, -chunkSize / 2.0 + cc.y * chunkSize}, {shiftX, 0, chunkSize / 2.0 + cc.y * chunkSize});
			lm->addLine({-chunkSize / 2.0 + cc.x * chunkSize, 0, shiftY}, {chunkSize / 2.0 + cc.x * chunkSize, 0, shiftY});
		}
		lm->drawLines();
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
		WDE_PROFILE_FUNCTION();

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
			while(itr != _removingChunks.end()) {
				auto id = itr->first;
				// Chunk is loading
				if (std::find(_loadingChunks.begin(), _loadingChunks.end(), id) != _loadingChunks.end()) {
					itr++;
					continue;
				}

				// Remove from active chunks
				if (_activeChunks.contains(id))
					_activeChunks.erase(id);
				itr++;
			}
			_removingChunks.clear();
		}
	}




	void WdeSceneInstance::reassignGOToChunks() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Reassigning game objects to nearest chunks." << logger::endl;

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

	void WdeSceneInstance::reorderGO() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Reordering game objects." << logger::endl;

		// For each chunks
		for (auto& c : _activeChunks) {
			// Copy game objects
			auto& activeGO = c.second->getGameObjects();
			std::unordered_map<resource::Material*, std::unordered_map<resource::Mesh*, std::vector<std::shared_ptr<GameObject>>>> res {};
			std::vector<std::shared_ptr<GameObject>> otherObjects {};

			// Order by materials and meshes
			for (auto& go : activeGO) {
				// Check if this game object will be rendered
				auto matMod = go->getModule<MeshRendererModule>();
				if (matMod == nullptr || matMod->getMaterial() == nullptr || matMod->getMesh() == nullptr) {
					otherObjects.push_back(go);
					continue;
				}

				// Create material if it doesn't exist
				auto mat = matMod->getMaterial();
				if (!res.contains(mat))
					res.emplace(mat, std::unordered_map<resource::Mesh*, std::vector<std::shared_ptr<GameObject>>> {});

				// Create mesh if it doesn't exit
				auto mesh = matMod->getMesh();
				if (!res.at(mat).contains(mesh))
					res.at(mat).emplace(mesh, std::vector<std::shared_ptr<GameObject>> {});

				// Add game object to material and mesh
				res.at(mat).at(mesh).push_back(go);
			}

			// Create new objects array ordered
			std::vector<std::shared_ptr<GameObject>> newGO {};
			std::vector<std::shared_ptr<GameObject>> newStaticGO {};
			std::vector<std::shared_ptr<GameObject>> newDynamicGO {};
			for (auto& matArr : res) {
				for (auto& meshArr : matArr.second) {
					for (auto& go : meshArr.second) {
						newGO.push_back(go);
						if (go->isStatic())
							newStaticGO.push_back(go);
						else
							newDynamicGO.push_back(go);
					}
				}
			}
			for (auto& go : otherObjects) {
				newGO.push_back(go);
				if (go->isStatic())
					newStaticGO.push_back(go);
				else
					newDynamicGO.push_back(go);
			}

			// Rewriting dynamic and static game objects
			c.second->getGameObjects() = newGO;
			c.second->getDynamicGameObjects() = newDynamicGO;
			c.second->getStaticGameObjects() = newStaticGO;
		}
	}
}
