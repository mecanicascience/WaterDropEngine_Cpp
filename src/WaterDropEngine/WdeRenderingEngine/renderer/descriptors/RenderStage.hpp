#pragma once

#include <tuple>
#include <vulkan/vulkan.h>

namespace wde::renderEngine {
	// Stage
	/** Represents a position in the render structure (first value is the render pass, second value is the sub-pass) */
	using RenderStage = std::pair<uint32_t, uint32_t>;
}
