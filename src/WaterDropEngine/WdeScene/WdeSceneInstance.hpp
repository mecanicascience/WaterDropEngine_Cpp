#pragma once

#include "../../wde.hpp"
#include "GameObject.hpp"
#include "../WdeCore/Structure/Observer.hpp"
#include "modules/CameraModule.hpp"
#include "terrain/Chunk.hpp"

namespace wde::scene {
	/**
	 * Engine scene instance
	 */
	class WdeSceneInstance : public core::Observer {
		public:
			// Scene instance methods
			/** Ticking for scene instance (called by WaterDropEngine) */
			void tick();
			void cleanUp();
			void onNotify(const core::Event& event) override;


			// Getters and setters
			void setPath(const std::string& path) { _scenePath = path; }
			const std::string& getPath() const { return _scenePath; }
			void setName(const std::string& name) { _sceneName = name; }
			const std::string& getName() const { return _sceneName; }

			GameObject*& getActiveGameObject() { return _selectedGameObject; }
			glm::ivec2 getSelectedGameObjectChunk() const { return _selectedGameObjectChunkID; }
			/** @return The first camera in the scene that is not the editor camera */
			GameObject* getFirstGameCamera() {
				for (auto& c : _activeChunks) {
					for (auto& go : c.second->getDynamicGameObjects()) {
						auto mod = go->getModule<CameraModule>();
						if (mod != nullptr && mod->getName() != "Editor Camera")
							return go.get();
					}
				}
				return nullptr;
			}
			void setActiveCamera(GameObject* camera) { _activeCamera = camera; }
			GameObject* getActiveCamera() const { return _activeCamera; }


			// Chunks manager
			/**
			 * @param chunkID Unique chunk position identifier
			 * @return The a pointer to the chunk
			 */
			Chunk* getChunk(glm::ivec2 chunkID) {
				// Chunk found
				if (_activeChunks.contains(chunkID))
					return _activeChunks.at(chunkID).get();

				// Not found
				auto ch = std::make_shared<Chunk>(chunkID);
				_activeChunks.emplace(chunkID, ch);
				return ch.get();
			}



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
			/** List of scene active chunks (pos - chunk*) */
			std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> _activeChunks {};
	};
}
