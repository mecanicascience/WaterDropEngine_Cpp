#include "CoreAppScene.hpp"

CoreAppScene::CoreAppScene() {
	std::cout << "Launch Scene" << std::endl;
}


void CoreAppScene::initialize() {
	std::cout << "Initialize Scene" << std::endl;
}

void CoreAppScene::update() {
	std::cout << "Update Scene" << std::endl;
}

void CoreAppScene::render() {
	std::cout << "Render Scene" << std::endl;
}

void CoreAppScene::cleanUp() {
	std::cout << "CleanUp Scene" << std::endl;
}
