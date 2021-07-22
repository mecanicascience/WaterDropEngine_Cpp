#pragma once

#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/Subrenderer.hpp"

namespace wde::renderEngine {
	class CoreAppSubrenderer : public Subrenderer {
		public:
			/** Creates the renderer given the stage */
			explicit CoreAppSubrenderer(const RenderStage &stage);

			/** Renders the sub-renderer on the provided command buffer */
			void render(const CommandBuffer &commandBuffer) override;
	};
}
