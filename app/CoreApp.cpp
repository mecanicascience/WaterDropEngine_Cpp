#include "CoreApp.hpp"


WdeStatus CoreApp::initialize() {
	// == Setup render engine ==
	// Choose Render Pipeline
	CoreAppPipeline graphicsPipeline {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"};

	// Choose Custom Renderer
	CoreAppRenderer renderer {};


	// Setup render engine data
	engine.setupRenderEngine(graphicsPipeline, renderer);


	// == Initialize engine ==
	WdeStatus status = engine.initialize();
	return status;
}
