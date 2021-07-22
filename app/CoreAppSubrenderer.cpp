#include "CoreAppSubrenderer.hpp"

namespace wde::renderEngine {
	CoreAppSubrenderer::CoreAppSubrenderer(const RenderStage &stage) : Subrenderer(stage) { }

	void CoreAppSubrenderer::render(const CommandBuffer &commandBuffer) {
		Logger::info("OMG JE RENDER.", LoggerChannel::RENDERING_ENGINE);
	}
}
