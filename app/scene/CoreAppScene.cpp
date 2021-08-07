#include "CoreAppScene.hpp"

void CoreAppScene::initialize() {
	// Add Camera
	auto& camera = createGameObject("Camera");
	camera.addModule<TransformControllerModule>();
	auto& cameraModule = camera.addModule<CameraModule>();
	cameraModule.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f)); // Camera look to the right
	cameraModule.setViewTarget(camera.getModule<TransformModule>().position, glm::vec3(0.0f, 0.0f, 0.0f));


	// Cube
	auto& cube = createGameObject("Cube");
	cube.addModule<ModelModule>(std::make_shared<ModelCube>());
	auto& cubeTransform = cube.getModule<TransformModule>();
	cubeTransform.position = {0.0f, 0.0f, 2.5f};
	cubeTransform.scale = {0.5f, 0.5f, 0.5f};

	// Test game object
	auto& testGO = createGameObject("Monkey Object");
	testGO.addModule<ModelModule>(std::make_shared<ModelLoader>("res/models/monkey_smooth.obj"));
	testGO.getModule<TransformModule>().scale /= 5.0f;
}

void CoreAppScene::update() {
	// Updates scene
	Scene::update();

	// Updates camera
	// _camera.update(getDeltaTime());
}
