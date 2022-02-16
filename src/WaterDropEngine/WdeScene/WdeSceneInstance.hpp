#pragma once

#include "../../wde.hpp"
#include "gameObjects/GameObject.hpp"
#include "../WdeCore/Structure/Observer.hpp"
#include "gameObjects/materials/Material.hpp"

namespace wde::scene {
	/**
	 * Engine scene instance
	 */
	class WdeSceneInstance : public core::Observer {
		public:
			// Scene engine methods
			virtual void setup() = 0;
			virtual void update() = 0;
			virtual void cleanUp() = 0;

			// Scene instance methods
			/** Ticking for scene instance (called by WaterDropEngine) */
			void tick();
			void onNotify(core::Event event) override;
			void drawGUIForGO(GameObject& go, int* selected, const std::string& depthIterator);



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

			/**
			 * Set the active camera ID
			 * @param go New game object (must have a camera)
			 */
			void setActiveCamera(const std::shared_ptr<GameObject>& go) {
				_activeCameraID = static_cast<int>(go->getID());
			}

			template<typename T, typename ...Args>
			std::shared_ptr<T> createMaterial(Args ...args) {
				_materials.push_back(std::make_shared<T>(args...));
				return std::static_pointer_cast<T>(_materials[_materials.size() - 1]);
			}


		private:
			// Scene containers
			/** List of scene game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjects;
			/** List of scene materials */
			std::vector<std::shared_ptr<Material>> _materials;

			// Scene utils
			/** Selected game object ID (none = -1) */
			int _selectedGameObjectID = -1;
			/** Active camera ID (none = -1) */
			int _activeCameraID = -1;
	};
}
