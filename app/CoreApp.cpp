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

	// Choose Render Pipeline
	wde::renderEngine::GraphicsDefaultPipeline graphicsPipeline {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"};

	// Choose renderer
	wde::renderEngine::DefaultRenderer renderer {};

	// Setup render engine datas
	engine.setupRenderEngine(graphicsPipeline, renderer, vertices, indices);


    // == Initialize engine ==
	WdeStatus status = engine.initialize();
    return status;
}
