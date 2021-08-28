#pragma once

class DrawGUIScene : public Scene {
	public:
		explicit DrawGUIScene() : Scene("Draw GUI Scene") {}

		/** Setup the scene elements */
		void initialize() override {
			/**
			 * Here, we add a simple game object and a camera to the scene, to see how the GUI reacts to this
			 */

			// == Add a camera game object ==
			auto& camera = createGameObject("Camera");
			camera.addModule<TransformControllerModule>(); // User can control camera with keyboard
			camera.addModule<CameraModule>(); // The camera viewing module


			// == Load the viking room game object at res/models/viking_room.obj ==
			auto& vikingGO = createGameObject("Viking room");
			vikingGO.addModule<ModelModule>(std::make_shared<ModelLoader>("viking_room.obj"), // Use viking room model
											std::make_shared<TextureMaterial>(RenderStage {0, 0}, "viking_room.png"));

			// Move the game object and rotate it
			vikingGO.getModule<TransformModule>().position = {0.0f, 0.0f, 0.5f};
			vikingGO.getModule<TransformModule>().rotation = {3 * glm::half_pi<float>(), 0.5f, 0.0f};
			vikingGO.getModule<TransformModule>().scale /= 3.0f;
		}
};

