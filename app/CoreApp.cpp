#include "CoreApp.hpp"
#include "WaterApp/WaterAppRenderer.hpp"
#include "WaterApp/WaterAppScene.hpp"
#include "ComputeAppTest/ComputeAppRenderer.hpp"
#include "ComputeAppTest/ComputeAppScene.hpp"


void CoreApp::start() {
	// Create render engine
	WaterDropEngine engine {};

	// Water app engine
	renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<WaterAppRenderer>());
	engine.initialize();
	scene::WdeSceneManager::get().setScene(std::make_unique<WaterAppScene>());

	// Compute app test
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<ComputeAppRenderer>());
	//engine.initialize();
	//scene::WdeSceneManager::get().setScene(std::make_unique<ComputeAppScene>());

	// Run engine
	engine.run();
}
