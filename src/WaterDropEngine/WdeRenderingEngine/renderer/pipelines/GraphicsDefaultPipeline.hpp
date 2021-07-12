#pragma once

#include "../GraphicsPipeline.hpp"

namespace wde::renderEngine {
	class GraphicsDefaultPipeline : public GraphicsPipeline {
		public:
			GraphicsDefaultPipeline(std::string shaderVertLocation, std::string shaderFragLocation) : GraphicsPipeline(shaderVertLocation, shaderFragLocation) { };

			/**
			 * @param width The width of the window
			 * @param height The height of the window
			 * @return The corresponding pipelines config infos
			 */
			void setPipelineConfigInfo(PipelineConfigInfo& configInfo);
	};
}
