#include "CoreApp.hpp"
#include "WaterApp/WaterAppRenderer.hpp"
#include "WaterApp/WaterAppScene.hpp"
#include "ComputeAppTest/ComputeAppRenderer.hpp"
#include "ComputeAppTest/ComputeAppScene.hpp"


void CoreApp::start() {
	// == Initialize engine ==
	// Create rendering engine
	WaterDropEngine engine {};

	// Set renderer
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<CoreAppRenderer>());
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<WaterAppRenderer>());
    renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<ComputeAppRenderer>());

	// Initialize main engine
	engine.initialize();

	// Set scene
	//scene::WdeSceneManager::get().setScene(std::make_unique<CoreAppScene>());
	//scene::WdeSceneManager::get().setScene(std::make_unique<WaterAppScene>());
    scene::WdeSceneManager::get().setScene(std::make_unique<ComputeAppScene>());


	// == Run engine ==
	engine.run();
}
