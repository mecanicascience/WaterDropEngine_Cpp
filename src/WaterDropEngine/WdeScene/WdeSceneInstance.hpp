#pragma once

#include "../../wde.hpp"
#include "GameObject.hpp"
#include "../WdeCore/Structure/Observer.hpp"
#include "modules/CameraModule.hpp"

namespace wde::scene {
	/**
	 * Engine scene instance
	 */
	class WdeSceneInstance : public core::Observer {
		public:
			// Scene engine override methods
			virtual void setup() {};
			virtual void update() {};
			virtual void cleanUp() {};

			// Scene instance methods
			/** Ticking for scene instance (called by WaterDropEngine) */
			void tick();
			void onNotify(const core::Event& event) override;
			void cleanUpInstance();


			// Getters and setters
			void setPath(const std::string& path) { _scenePath = path; }
			const std::string& getPath() const { return _scenePath; }
			void setName(const std::string& name) { _sceneName = name; }
			const std::string& getName() const { return _sceneName; }

			std::vector<std::shared_ptr<GameObject>>& getGameObjects() { return _gameObjects; }
			std::vector<std::shared_ptr<GameObject>>& getStaticGameObjects()  { return _gameObjectsStatic; }
			std::vector<std::shared_ptr<GameObject>>& getDynamicGameObjects() { return _gameObjectsDynamic; }

			GameObject* getActiveGameObject() const { return _selectedGameObject; }
			/** @return The first camera in the scene that is not the editor camera */
			GameObject* getFirstGameCamera() {
				for (auto& go : _gameObjectsDynamic) {
					auto mod = go->getModule<CameraModule>();
					if (mod != nullptr && mod->getName() != "Editor Camera")
						return go.get();
				}
				return nullptr;
			}
			void setActiveCamera(GameObject* camera) { _activeCamera = camera; }
			GameObject* getActiveCamera() const { return _activeCamera; }


			/**
			 * Create a new GameObject
			 * @param name Name of the GameObject
			 * @param isStatic True if the game object is a static one (default false)
			 */
			std::shared_ptr<GameObject> createGameObject(const std::string& name, bool isStatic = false) {
				auto goPtr = std::make_shared<GameObject>(_gameObjectsIDCurr++, name, isStatic);
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


		private:
			// Scene game objects
			/** List of all scene game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjects;
			/** List of scene static game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsStatic;
			/** List of scene dynamic game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsDynamic;
			/** List of all scene game objects to delete on next tick */
			std::vector<GameObject*> _gameObjectsToDelete {};

			// Scene utils
			/** Path to the scene object */
			std::string _scenePath;
			/** Name of the scene object */
			std::string _sceneName;
			/** Last create game object ID */
			uint32_t _gameObjectsIDCurr = 0;

			// Selected game objects
			/** Selected game object for GUI (none : nullptr) */
			GameObject* _selectedGameObject = nullptr;
			/** Active camera (none = nullptr) */
			GameObject* _activeCamera = nullptr;

			// Helper functions
			void drawGUIForGo(const std::shared_ptr<GameObject>& go, GameObject*& selected) const;
	};
}
