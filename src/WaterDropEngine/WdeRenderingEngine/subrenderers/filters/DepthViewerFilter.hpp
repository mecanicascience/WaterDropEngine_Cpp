#pragma once

#include "FilterSubrenderer.hpp"

namespace wde::renderEngine {
	/**
	 * Filter that render a depth given attachment
	 * @param stage The render stage of the filter
	 * @param attachmentBindingIndex The index of the depth attachment
	 */
	class DepthViewerFilter : public FilterSubrenderer {
		/**
		 * Depth push data
		 */
		struct PushConstantDepthData {
			float minDepth = 0.0f;
			float maxDepth = 100.0f;
		};


		public:
			/**
			 * @param stage
			 * @param attachmentBindingIndex
			 */
			explicit DepthViewerFilter(const RenderStage &stage, uint32_t attachmentBindingIndex)
				: FilterSubrenderer(stage, "depthViewerFilter.frag.spv", { attachmentBindingIndex }) {}

			/** Initialize the depth filter */
			void initialize() override {
				// Setup push constants
				_pipeline.addPushConstants(0, sizeof(PushConstantDepthData));

				// Initialize the filter
				FilterSubrenderer::initialize();
			}
	};
}

