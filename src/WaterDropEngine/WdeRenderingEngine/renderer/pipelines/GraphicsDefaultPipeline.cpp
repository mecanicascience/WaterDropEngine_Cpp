#include "GraphicsDefaultPipeline.hpp"

namespace wde::renderEngine {
	GraphicsPipeline::PipelineConfigInfo GraphicsDefaultPipeline::getPipelineConfigInfo(uint32_t width, uint32_t height) {
		// Uses the default graphics pipelines infos
		return getDefaultPipelineConfigInfo(width, height);
	}
}
