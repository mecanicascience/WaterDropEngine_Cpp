#include "CoreAppSubrenderer.hpp"

namespace wde::renderEngine {
	CoreAppSubrenderer::CoreAppSubrenderer(const RenderStage &stage)
		: Subrenderer(stage), _pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"}, { Model::Vertex::getDescriptions() },
          PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE) { }

	void CoreAppSubrenderer::render(const CommandBuffer &commandBuffer) {
		// Binds to the graphics pipeline
		_pipeline.bind(commandBuffer);

		// Draws the model
		_model->render(commandBuffer);
	}
}
