#pragma once

#include <vector>

#include "../../../wde.hpp"
#include "../objects/GameObject.hpp"

namespace wde::scene {
	class Scene : NonCopyable {
		public:
			// Core functions
			/** Initialize scene */
			virtual void initialize() = 0;

			/** Updates the scene (this should be called from the derived children class) */
			virtual void update() {
				// Updates delta time
				auto newTime = std::chrono::high_resolution_clock::now();
				_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - _currentTime).count();
				_currentTime = newTime;

				// Updates game objects
				for (auto& go : _gameObjects)
					go.update(_deltaTime);
			}

			/** Cleaning up scene */
			virtual void cleanUp() final {
				// Clean up game objects
				for (auto& go : _gameObjects)
					go.cleanUp();
				_gameObjects.clear();
			}

			// Getters and setters
			void addGameObject(GameObject& gameObject) { _gameObjects.push_back(gameObject); };
			std::vector<GameObject>& getGameObjects() { return _gameObjects; }


		protected:
			/** @return the delta time since the last update (in seconds) */
			const float getDeltaTime() const { return _deltaTime; }

		private:
			/** The list of the scene game objects */
			std::vector<GameObject> _gameObjects {};

			/** Stores the current time to compute delta time */
			std::chrono::time_point<std::chrono::system_clock> _currentTime = std::chrono::high_resolution_clock::now();
			/** Last computed delta time */
			float _deltaTime = 0;
	};
}
