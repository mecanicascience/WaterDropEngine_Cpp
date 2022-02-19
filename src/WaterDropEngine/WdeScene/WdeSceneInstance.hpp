#pragma once

#include "../../wde.hpp"
#include "gameObjects/GameObject.hpp"
#include "../WdeCore/Structure/Observer.hpp"
#include "gameObjects/materials/Material.hpp"
#include "gameObjects/meshes/Mesh.hpp"

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
			void onNotify(core::Event event) override;
			void drawGUIForGO(GameObject& go, int* selected, const std::string& depthIterator);
			void cleanUpInstance() {
			    _materials.clear();
			    _meshes.clear();
			    _gameObjects.clear();

			    // Clean up scene
                cleanUp();
			}


			// Getters and setters
			std::vector<std::shared_ptr<Material>>& getMaterials() { return _materials; }
			GameObject& getGameObject(int goID) { return *_gameObjects[goID]; }
			std::vector<std::shared_ptr<GameObject>>& getGameObjects() { return _gameObjects; }
			std::shared_ptr<GameObject> getActiveCamera() {
				if (_activeCameraID == -1)
					return nullptr;
				return _gameObjects[_activeCameraID];
			}
			void setActiveCamera(int gameObjectID) { _activeCameraID = gameObjectID; }


	protected:
			/**
			 * Create a new GameObject
			 * @param name Name of the GameObject
			 */
			std::shared_ptr<GameObject> createGameObject(const std::string& name) {
				static uint32_t ID = 0;
				_gameObjects.push_back(std::make_shared<GameObject>(ID++, name));
				return _gameObjects[_gameObjects.size() - 1];
			}

			template<typename T, typename ...Args>
			std::shared_ptr<T> createMaterial(Args ...args) {
				_materials.push_back(std::make_shared<T>(args...));
				return std::static_pointer_cast<T>(_materials[_materials.size() - 1]);
			}

            template<typename T, typename ...Args>
            std::shared_ptr<T> createMesh(Args ...args) {
                _meshes.push_back(std::make_shared<T>(args...));
                return std::static_pointer_cast<T>(_meshes[_meshes.size() - 1]);
            }


		private:
			// Scene containers
			/** List of scene game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjects;
			/** List of scene materials */
			std::vector<std::shared_ptr<Material>> _materials;
            /** List of scene meshes */
            std::vector<std::shared_ptr<Mesh>> _meshes;

			// Scene utils
			/** Selected game object ID (none = -1) */
			int _selectedGameObjectID = -1;
			/** Active camera ID (none = -1) */
			int _activeCameraID = -1;
	};
}
