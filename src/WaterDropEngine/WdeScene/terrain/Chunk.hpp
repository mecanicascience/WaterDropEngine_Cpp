#pragma once

#include "../../../wde.hpp"
#include "../GameObject.hpp"
#include "../modules/MeshRendererModule.hpp"
#include "../modules/CameraModule.hpp"

#include <utility>

namespace wde::scene {
	class WdeSceneInstance;

	/**
	 * Class that represents a scene terrain chunk.
	 */
	class Chunk {
		public:
			// Constructors
			explicit Chunk(WdeSceneInstance* sceneInstance, glm::ivec2 pos);
			~Chunk();

			/** Saves the chunk data to the associated chunk file */
			void save();
			void tick();
			void drawGUI();


			// Getters
			std::vector<std::shared_ptr<GameObject>>& getGameObjects() { return _gameObjects; }
			std::vector<std::shared_ptr<GameObject>>& getStaticGameObjects()  { return _gameObjectsStatic; }
			std::vector<std::shared_ptr<GameObject>>& getDynamicGameObjects() { return _gameObjectsDynamic; }
			static void setObjectIDCurrent(uint32_t id) { _gameObjectsIDCurr = id; }


			// Game Objects Manager
			/**
			 * Add a pre-existing game object to the list
			 * @param go
			 */
			void addGameObject(const std::shared_ptr<GameObject>& go) {
				// Add to list
				_gameObjects.push_back(go);
				if (go->isStatic())
					_gameObjectsStatic.push_back(go);
				else
					_gameObjectsDynamic.push_back(go);
			}
			/**
			 * Create a new GameObject
			 * @param name Name of the GameObject
			 * @param isStatic True if the game object is a static one (default false)
			 */
			std::shared_ptr<GameObject> createGameObject(const std::string& name, bool isStatic = false) {
				auto goPtr = std::make_shared<GameObject>(Chunk::_gameObjectsIDCurr++, name, isStatic);
				_gameObjects.push_back(goPtr);
				if (isStatic)
					_gameObjectsStatic.push_back(goPtr);
				else
					_gameObjectsDynamic.push_back(goPtr);
				return goPtr;
			}

			/**
			 * Remove a given GameObject
			 * @param go
			 */
			void removeGameObject(GameObject* go) {
				// Remove GameObject
				_gameObjectsToDelete.push_back(go);
			}
			/** Clear the game objects list */
			void clearGameObjects() {
				_gameObjects.clear();
				_gameObjectsStatic.clear();
				_gameObjectsDynamic.clear();
			}


		private:
			// Chunk data
			WdeSceneInstance* _sceneInstance;
			glm::ivec2 _pos;

			// Chunk game objects handling
			/** List of all scene game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjects {};
			/** List of scene static game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsStatic {};
			/** List of scene dynamic game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsDynamic {};
			/** List of all scene game objects to delete on next tick */
			std::vector<GameObject*> _gameObjectsToDelete {};

			// Chunk game objects data
			/** Last create game object ID */
			static uint32_t _gameObjectsIDCurr;

			
			// Helper functions
			/**
			 * Draw the GUI for a given game object
			 * @param go GameObject
			 * @param selected True if the GameObject is selected
			 */
			void drawGUIForGo(const std::shared_ptr<GameObject>& go, GameObject*& selected) const;
	};
}
