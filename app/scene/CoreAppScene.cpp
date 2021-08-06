#include "CoreAppScene.hpp"


void CoreAppScene::initialize() {
	auto cube = GameObject::createGameObject("Cube");
	auto& cubeTransform = cube.getModule<TransformModule>();
	cubeTransform.position = {0.0f, 0.0f, 2.5f};
	cubeTransform.scale = {0.5f, 0.5f, 0.5f};
	cube.model = std::make_shared<ModelCube>();
	addGameObject(cube);

	auto testGO = GameObject::createGameObject("Monkey Object");
	testGO.model = std::make_shared<ModelLoader>("res/models/monkey_smooth.obj");
	testGO.getModule<TransformModule>().scale /= 5.0f;
	addGameObject(testGO);
}

void CoreAppScene::update() {
	// Updates scene
	Scene::update();
	// float dt = getDeltaTime();
}
