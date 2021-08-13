#pragma once

#include "../renderer/Subrenderer.hpp"
#include "../../WdeScene/WdeSceneManager.hpp"
#include "../pipelines/PipelineGraphics.hpp"

namespace wde::renderEngine {
	/**
	 * A simple image filter
	 */
	class SimpleFilterSubrenderer : public Subrenderer {
		public:
			// Constructors
			/**
			 * Create the filter at the given stage
			 * @param stage
			 */
			explicit SimpleFilterSubrenderer(const RenderStage &stage);

			/** Clean up sub-renderer */
			~SimpleFilterSubrenderer() override = default;


			// Core functions
			/** Initialize the sub-renderer pipeline and descriptor */
			void initialize() override;
			/** Renders the filter */
			void render(CommandBuffer &commandBuffer) override;


		private:
			PipelineGraphics _pipeline;
			std::shared_ptr<Descriptor> _descriptor;
	};
}

