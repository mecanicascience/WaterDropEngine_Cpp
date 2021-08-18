#include "CoreApp.hpp"
#include "WaterApp/WaterAppRenderer.hpp"
#include "WaterApp/WaterAppScene.hpp"


void CoreApp::start() {
	// == Initialize engine ==
	// Create rendering engine
	WaterDropEngine engine {};

	// Set renderer
	renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<CoreAppRenderer>());
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<WaterAppRenderer>());

	// Initialize main engine
	engine.initialize();

	// Set scene
	scene::WdeSceneManager::get().setScene(std::make_unique<CoreAppScene>());
	//scene::WdeSceneManager::get().setScene(std::make_unique<WaterAppScene>());


	// == Run engine ==
	engine.run();
}
