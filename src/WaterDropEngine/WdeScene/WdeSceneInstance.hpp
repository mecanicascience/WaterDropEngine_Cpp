#pragma once

#include <queue>

#include "../../wde.hpp"
#include "GameObject.hpp"
#include "../WdeCore/Structure/Observer.hpp"
#include "modules/CameraModule.hpp"
#include "terrain/Chunk.hpp"
#include "../WdeGUI/panels/WorldPartitionPanel.hpp"

namespace wde::scene {
	/**
	 * Engine scene instance
	 */
	class WdeSceneInstance : public core::Observer {
		public:
			// Scene instance methods
			WdeSceneInstance();

			/** Ticking for scene instance (called by WaterDropEngine) */
			void tick();
			void cleanUp();
			void onNotify(const core::Event& event) override;
			void drawGizmo(scene::Gizmo& gizmo, render::CommandBuffer& commandBuffer);


			// Getters and setters
			void setPath(const std::string& path) { _scenePath = path; }
			const std::string& getPath() const { return _scenePath; }
			void setName(const std::string& name) { _sceneName = name; }
			const std::string& getName() const { return _sceneName; }
			gui::WorldPartitionPanel& getWorldPartitionPanel() { return *_worldPartitionPanel; }

			GameObject*& getActiveGameObject() { return _selectedGameObject; }
			glm::ivec2 getSelectedGameObjectChunk() const { return _selectedGameObjectChunkID; }
			/** @return The first camera in the scene that is not the editor camera */
			GameObject* getFirstGameCamera() {
				for (auto& c : _activeChunks) {
					for (auto& go : c.second->getDynamicGameObjects()) {
						auto mod = go->getModule<CameraModule>();
						if (mod != nullptr && go->name != "Editor Camera")
							return go.get();
					}
				}
				return nullptr;
			}
			void setActiveCamera(GameObject* camera) { _activeCamera = camera; }
			GameObject* getActiveCamera() const { return _activeCamera; }

			// Chunks manager
			std::vector<glm::ivec2>& getLoadingChunks() { return _loadingChunks; }
			std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& getActiveChunks() { return _activeChunks; }
			std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& getUnloadingChunks() { return _removingChunks; }
			glm::ivec2 getCurrentChunkID() const {
				double chunkSize = Config::CHUNK_SIZE;
				glm::ivec2 cc { 0, 0 };
				if (_activeCamera != nullptr) {
					cc.x = std::floor(_activeCamera->transform->position.x / chunkSize + 0.5);
					cc.y = std::floor(_activeCamera->transform->position.z / chunkSize + 0.5);
				}
				return {cc.x, cc.y};
			}
			std::pair<VkDescriptorSet, VkDescriptorSetLayout>& getDefaultGlobalSet() { return _globalSetDefault; }

			/**
			 * @param chunkID Unique chunk position identifier
			 * @return The pointer to the chunk (nullptr if added to load list)
			 */
			Chunk* getChunk(glm::ivec2 chunkID);

			/**
			 * Loads a chunk synchronously
			* @param chunkID Unique chunk position identifier
			* @return The pointer to the chunk
			*/
			Chunk* getChunkSync(glm::ivec2 chunkID);

			/**
			 * Remove a chunk from the list if the chunk exists
			 * @param chunkID
			 */
			void removeChunk(glm::ivec2 chunkID);

			/** Create and unload chunks */
			void manageChunks();
			/** Reassign game objects to nearest chunk */
			void reassignGOToChunks();



		private:
			// Scene utils
			/** Path to the scene object */
			std::string _scenePath;
			/** Name of the scene object */
			std::string _sceneName;


			// Selected game objects
			/** Selected game object chunk id */
			glm::ivec2 _selectedGameObjectChunkID {0, 0};
			/** Selected game object for GUI (none : nullptr) */
			GameObject* _selectedGameObject = nullptr;
			/** Active camera (none = nullptr) */
			GameObject* _activeCamera = nullptr;


			// Scene chunks
			/** List of scene loading chunks (pos - chunk*) */
			std::vector<glm::ivec2> _loadingChunks {};
			/** List of scene active chunks (pos - chunk*) */
			std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> _activeChunks {};
			/** Lists of chunks that needs to be deleted (pos - chunk*) */
			std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> _removingChunks {};
			/** Global default set */
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _globalSetDefault {};


			// GUI
			/** The panel that displays loaded chunks list */
			std::unique_ptr<gui::WorldPartitionPanel> _worldPartitionPanel {};
			std::unique_ptr<render::Buffer> _cameraData {};
			std::unique_ptr<render::Buffer> _objectsData {};
	};
}
