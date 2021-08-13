#pragma once

#include "FilterSubrenderer.hpp"

namespace wde::renderEngine {
	/**
	 * Filter that copy an attachment to an other
	 * @param stage The render stage of the filter
	 * @param attachmentBindingIndex The index of the attachment to apply the filter on
	 */
	class DefaultFilter : public FilterSubrenderer {
		public:
			explicit DefaultFilter(const RenderStage &stage, uint32_t attachmentBindingIndex)
				: FilterSubrenderer(stage, "defaultFilter.frag.spv", { attachmentBindingIndex }) {}
	};
}

