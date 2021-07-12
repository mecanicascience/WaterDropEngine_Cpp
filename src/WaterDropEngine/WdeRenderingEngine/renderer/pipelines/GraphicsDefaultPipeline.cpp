#include "GraphicsDefaultPipeline.hpp"

namespace wde::renderEngine {
	void GraphicsDefaultPipeline::setPipelineConfigInfo(PipelineConfigInfo& configInfo) {
		// Uses the default graphics pipelines infos
		setDefaultPipelineConfigInfo(configInfo);
	}
}
