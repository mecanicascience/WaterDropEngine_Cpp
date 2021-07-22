#pragma once

#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/Subrenderer.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/structures/Model.hpp"

namespace wde::renderEngine {
	class CoreAppSubrenderer : public Subrenderer {
		public:
			/** Creates the renderer given the stage */
			explicit CoreAppSubrenderer(const RenderStage &stage);

			/** Records commands on the given command buffer */
			void render(const CommandBuffer &commandBuffer) override;

			// Set model
			void setModel(std::unique_ptr<Model> &&model) { _model = std::move(model); }

		private:
			/** The rendering graphics pipeline */
			PipelineGraphics _pipeline;
			/** The element model */
			std::unique_ptr<Model> _model;
	};
}
