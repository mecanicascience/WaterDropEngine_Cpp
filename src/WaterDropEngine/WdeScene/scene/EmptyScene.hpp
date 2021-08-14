#pragma once

#include "../Scene.hpp"
#include "../modules/CameraModule.hpp"

namespace wde::scene {
	/**
	 * Class that represents a simple empty scene
	 */
	class EmptyScene : public Scene {
		public:
			EmptyScene() : Scene("Empty Scene") {}

			/** Add waiting game objects to the screen */
			void initialize() override {
				// Add a simple camera game object
				auto& camera = createGameObject("Camera");
				camera.addModule<CameraModule>();
			}
	};
}
