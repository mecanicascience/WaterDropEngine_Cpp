#include "ExamplesApp.hpp"

void ExamplesApp::start() {
	// == Create render engine ==
	WaterDropEngine engine {};


	// == 01 - Simple triangle ==
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<DrawSimpleTriangleRenderer>());  // Set renderer
	//engine.initialize();  // Initialize the engine

	// == 02 - Draw cube ==
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<DrawCubeRenderer>());  // Set renderer
	//engine.initialize();  // Initialize the engine
	//scene::WdeSceneManager::get().setScene(std::make_unique<DrawCubeScene>());  // Set scene

	// == 03 - Load shapes ==
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<LoadShapesRenderer>());  // Set renderer
	//engine.initialize();  // Initialize the engine
	//scene::WdeSceneManager::get().setScene(std::make_unique<LoadShapesScene>());  // Set scene

	// == 04 - Draw GUI ==
	//renderEngine::WdeRenderEngine::get().setRenderer(std::make_unique<DrawGUIRenderer>());  // Set renderer
	//engine.initialize();  // Initialize the engine
	//scene::WdeSceneManager::get().setScene(std::make_unique<DrawGUIScene>());  // Set scene



	// == Run engine ==
	engine.run();
}
