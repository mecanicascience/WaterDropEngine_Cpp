#pragma once

#include "FilterSubrenderer.hpp"

namespace wde::renderEngine {
	/**
	 * Filter that invert colors of an attachment
	 * @param stage The render stage of the filter
	 * @param attachmentBindingIndex The index of the attachment to apply the filter on
	 */
	class InvertFilter : public FilterSubrenderer {
		public:
			explicit InvertFilter(const RenderStage &stage, uint32_t attachmentBindingIndex)
				: FilterSubrenderer(stage, "invertFilter.frag.spv", { attachmentBindingIndex }) {}
	};
}

