#include "CoreAppScene.hpp"

void CoreAppScene::initialize() {
	// Add Camera
	_camera = GameObject::createGameObject("Camera");
	_camera.addModule<TransformControllerModule>();
	auto& cameraModule = _camera.addModule<CameraModule>();
	//cameraModule.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f)); // Camera look to the right
	cameraModule.setViewTarget(_camera.getModule<TransformModule>().position, glm::vec3(0.0f, 0.0f, 0.0f));
	addGameObject(_camera);


	// Cube
	auto cube = GameObject::createGameObject("Cube");
	cube.addModule<ModelModule>(std::make_shared<ModelCube>());
	auto& cubeTransform = cube.getModule<TransformModule>();
	cubeTransform.position = {0.0f, 0.0f, 2.5f};
	cubeTransform.scale = {0.5f, 0.5f, 0.5f};
	addGameObject(cube);

	// Test game object
	auto testGO = GameObject::createGameObject("Monkey Object");
	testGO.addModule<ModelModule>(std::make_shared<ModelLoader>("res/models/monkey_smooth.obj"));
	testGO.getModule<TransformModule>().scale /= 5.0f;
	addGameObject(testGO);
}

void CoreAppScene::update() {
	// Updates scene
	Scene::update();

	// Updates camera
	_camera.update(getDeltaTime());
}
