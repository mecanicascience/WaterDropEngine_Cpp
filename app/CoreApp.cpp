#include "CoreApp.hpp"


WdeStatus CoreApp::initialize() {
	// == Initialize engine ==
	// Create rendering engine
	WaterDropEngine engine {};

	// Set renderer
	renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<CoreAppRenderer>());

	// Initialize main engine
	WdeStatus statusInit = engine.initialize();
	if (statusInit != WdeStatus::WDE_SUCCESS)
		return statusInit;


	// == Run engine ==
	WdeStatus statusRun = engine.run();
	if (statusRun != WdeStatus::WDE_SUCCESS)
		return statusRun;

	return WdeStatus::WDE_SUCCESS;
}
