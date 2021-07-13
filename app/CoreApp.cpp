#include "CoreApp.hpp"


WdeStatus CoreApp::initialize() {
	// == Setup render engine ==
	// Create Model data
	std::vector<wde::renderEngine::Model::Vertex> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };



	// Choose Render Pipeline and renderer
	CoreAppPipeline graphicsPipeline {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"};
	CoreAppRenderer renderer {};


	// Setup render engine data
	engine.setupRenderEngine(graphicsPipeline, renderer, vertices, indices);


    // == Initialize engine ==
	WdeStatus status = engine.initialize();
    return status;
}
