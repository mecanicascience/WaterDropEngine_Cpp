#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

namespace wde::Constants {
	extern const std::string APPLICATION_NAME = "WaterDrop Engine";

	extern const std::vector APPLICATION_VERSION_RAW {0, 1, 2}; // In order major, minor, patch
	extern const uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(APPLICATION_VERSION_RAW[0],
													APPLICATION_VERSION_RAW[1], APPLICATION_VERSION_RAW[2]);
	extern const std::string APPLICATION_VERSION_FORMATTED = "v" + std::to_string(APPLICATION_VERSION_RAW[0]) +  "."
			+ std::to_string(APPLICATION_VERSION_RAW[1]) + "." + std::to_string(APPLICATION_VERSION_RAW[2]);
}
