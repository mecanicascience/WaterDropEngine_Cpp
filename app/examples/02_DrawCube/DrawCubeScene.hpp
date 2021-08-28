#pragma once

class DrawCubeScene : public Scene {
	public:
		explicit DrawCubeScene() : Scene("Draw Cube Scene") {}

		/** Setup the scene elements */
		void initialize() override {
			// == Add a camera game object ==
			auto& camera = createGameObject("Camera");
			camera.addModule<TransformControllerModule>(); // User can control camera with keyboard
			camera.addModule<CameraModule>(); // The camera viewing module


			// == Add a cube game object ==
			auto& cube = createGameObject("Cube");

			// Set the cube material as a color material
			cube.addModule<ModelModule>(
					std::make_shared<ModelCube>(), // Cube model

					// Cube material
			        std::make_shared<ColorMaterial>(RenderStage {0, 0})  // Draw the cube faces color
			        //std::make_shared<NormalMaterial>(RenderStage {0, 0}) // Draw the cube normal orientation
			        //std::make_shared<DepthMaterial>(RenderStage {0, 0})  // Draw the cube depth texture
	        );

			// Move, scale and rotate the cube
			auto& cubeTransform = cube.getModule<TransformModule>();
			cubeTransform.position = {0.0f, 0.0f, 2.5f};
			cubeTransform.scale /= 2.0f;
			cubeTransform.rotation = {2.1f, 1.05f, 0.0f};
		}
};

