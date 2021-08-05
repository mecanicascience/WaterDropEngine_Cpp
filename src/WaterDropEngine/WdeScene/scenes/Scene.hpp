#pragma once

#include <vector>

#include "../../../wde.hpp"
#include "../objects/GameObject.hpp"

namespace wde::scene {
	class Scene : NonCopyable {
		public:
			// Core functions
			virtual void initialize() = 0;
			virtual void update() = 0;
			virtual void cleanUp() final {
				// Clean up game objects
				for (auto& go : _gameObjects)
					go.cleanUp();
				_gameObjects.clear();
			}

			// Getters and setters
			void addGameObject(GameObject& gameObject) { _gameObjects.push_back(gameObject); };
			std::vector<GameObject>& getGameObjects() { return _gameObjects; }


		private:
			/** The list of the scene game objects */
			std::vector<GameObject> _gameObjects {};
	};
}
