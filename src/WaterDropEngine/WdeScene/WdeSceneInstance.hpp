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
			/** Active camera ID (none = -1) */
			int _activeCameraID = -1;

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
			const std::shared_ptr<GameObject>& getGameObject(int goID) { return _gameObjects[goID]; }
			std::vector<std::shared_ptr<GameObject>>& getGameObjects() { return _gameObjects; }
			std::vector<std::shared_ptr<GameObject>>& getStaticGameObjects()  { return _gameObjectsStatic; }
			std::vector<std::shared_ptr<GameObject>>& getDynamicGameObjects() { return _gameObjectsDynamic; }
			std::shared_ptr<GameObject> getActiveGameObject() const {
				if (_selectedGameObjectID == -1)
					return nullptr;
				return _gameObjects[_selectedGameObjectID];
			}
			std::shared_ptr<GameObject> getFirstGameCamera() const {
				for (auto& go : _gameObjects) {
					auto mod = go->getModule<CameraModule>();
					if (mod != nullptr && mod->getName() != "Editor Camera")
						return go;
				}
				return nullptr;
			}
			std::shared_ptr<GameObject> getActiveCamera() const {
				if (_activeCameraID == -1)
					return nullptr;
				return _gameObjects[_activeCameraID];
			}


			/**
			 * Create a new GameObject
			 * @param name Name of the GameObject
			 * @param isStatic True if the game object is a static one (default false)
			 */
			std::shared_ptr<GameObject> createGameObject(const std::string& name, bool isStatic = false) {
				_gameObjects.push_back(std::make_shared<GameObject>(_gameObjectsIDCurr++, name, isStatic));
				if (isStatic)
					_gameObjectsStatic.push_back(_gameObjects[_gameObjects.size() - 1]);
				else
					_gameObjectsDynamic.push_back(_gameObjects[_gameObjects.size() - 1]);
				return _gameObjects[_gameObjects.size() - 1];
			}


		private:
			// Scene game objects
			/** List of all scene game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjects;
			/** List of scene static game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsStatic;
			/** List of scene dynamic game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsDynamic;

			// Scene utils
			/** Path to the scene object */
			std::string _scenePath;
			/** Name of the scene object */
			std::string _sceneName;
			/** Selected game object ID for GUI (default : 0) */
			int _selectedGameObjectID = 0;
			/** Last create game object ID */
			uint32_t _gameObjectsIDCurr = 0;

			// Helper functions
			void drawGUIForGo(const std::shared_ptr<GameObject>& go, int* selected) const;
	};
}
