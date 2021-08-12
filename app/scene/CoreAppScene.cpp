#include "CoreAppScene.hpp"

void CoreAppScene::initialize() {
	// Add Camera
	auto& camera = createGameObject("Camera");
	camera.addModule<TransformControllerModule>();
	camera.addModule<CameraModule>();

	// Materials
	// ColorMaterial - ColorMaterialOutline - TextureMaterial

	// Cube
	auto& cube = createGameObject("Cube");
	cube.addModule<ModelModule>(std::make_shared<ModelCube>(), std::make_shared<TextureMaterial>(RenderStage {0, 0}, "test-texture.jpg"));
	auto& cubeTransform = cube.getModule<TransformModule>();
	cubeTransform.position = {0.0f, 0.0f, 2.5f};
	cubeTransform.scale /= 2.0f;

	// Test game object
	/*auto& testGO = createGameObject("Monkey Object");
	testGO.addModule<ModelModule>(std::make_shared<ModelLoader>("res/models/monkey_smooth.obj"), std::make_shared<ColorMaterial>(RenderStage {0, 0}));
	testGO.getModule<TransformModule>().scale /= 5.0f;*/
}

void CoreAppScene::update() {
	// Updates scene
	Scene::update();
}
