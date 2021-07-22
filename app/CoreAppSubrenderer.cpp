#include "CoreAppSubrenderer.hpp"

namespace wde::renderEngine {
	CoreAppSubrenderer::CoreAppSubrenderer(const RenderStage &stage)
		: Subrenderer(stage), pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"}) { }

	void CoreAppSubrenderer::render(const CommandBuffer &commandBuffer) {
		// Binds to the graphics pipeline
		pipeline.bind(commandBuffer);

		// Tell to draw the triangle
		vkCmdDraw(commandBuffer,
		          3, // 3 vertices to draw
		          1, // instanced rendering (1 if not doing that)
		          0, // Offset into vertex buffer
		          0); // Offset into instanced rendering
	}
}
