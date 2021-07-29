#include "CoreAppSubrenderer.hpp"

namespace wde::renderEngine {
	struct SimplePushConstantData {
		glm::mat2 transform {1.0f};
		glm::vec2 offset;
	};

	CoreAppSubrenderer::CoreAppSubrenderer(const RenderStage &stage)
			: Subrenderer(stage),
			  _pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"}, { Model::Vertex::getDescriptions() },
						PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		                VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE) {
		// Set pipeline push constants infos
		_pipeline.addPushConstants(0, sizeof(SimplePushConstantData));

		// Initialize pipeline
		_pipeline.initialize();
	}

	void CoreAppSubrenderer::render(CommandBuffer &commandBuffer) {
		// Update game objects
		int i = 0;
		for (auto &obj : _gameObjects) {
			i++;
			obj.transform2D.rotation += 0.001f/2.0f * i;
		}


		// Binds the graphics pipeline and model to the command buffer
		_pipeline.bind(commandBuffer);

		// Render game objects
		for (auto &obj : _gameObjects) {
			// Setup push constants
			SimplePushConstantData push {};
			push.offset = obj.transform2D.translation;
			push.transform = obj.transform2D.mat2();
			_pipeline.setPushConstants(0, &push);

			// Render game object
			obj.render(commandBuffer);
		}
	}
}
