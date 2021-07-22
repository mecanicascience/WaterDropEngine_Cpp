#pragma once

#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/Subrenderer.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.hpp"

namespace wde::renderEngine {
	class CoreAppSubrenderer : public Subrenderer {
		public:
			/** Creates the renderer given the stage */
			explicit CoreAppSubrenderer(const RenderStage &stage);

			/** Records commands on the given command buffer */
			void render(const CommandBuffer &commandBuffer) override;

		private:
			/** The rendering graphics pipeline */
			PipelineGraphics pipeline;
	};
}
