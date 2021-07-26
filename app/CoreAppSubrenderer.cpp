#include "CoreAppSubrenderer.hpp"

namespace wde::renderEngine {
	CoreAppSubrenderer::CoreAppSubrenderer(const RenderStage &stage)
		: Subrenderer(stage), _pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"},
                                            { Model::Vertex::getDescriptions() }, PipelineGraphics::Depth::ReadWrite) { }

	void CoreAppSubrenderer::render(const CommandBuffer &commandBuffer) {
		// Binds to the graphics pipeline
		_pipeline.bind(commandBuffer);

		// Draws the model
		_model->render(commandBuffer);
	}
}
