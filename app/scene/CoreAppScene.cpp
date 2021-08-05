#include "CoreAppScene.hpp"


void CoreAppScene::initialize() {
	/*auto cube = GameObject::createGameObject();
	cube.model = std::make_shared<ModelCube>();
	cube.transform.translation = {0.0f, 0.0f, 2.5f};
	cube.transform.scale = {0.5f, 0.5f, 0.5f};
	cubeSubrenderer->addGameObject(cube);*/

	auto testGO = GameObject::createGameObject();
	testGO.model = std::make_shared<ModelLoader>("res/models/monkey_smooth.obj");
	testGO.transform.scale /= 5.0f;
	addGameObject(testGO);
}

void CoreAppScene::update() {
	auto newTime = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - _currentTime).count();
	_currentTime = newTime;
}
