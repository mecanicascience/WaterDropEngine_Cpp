#include "Config.hpp"

namespace wde::Config {
	/** LOG OUTPUT LEVEL (0 = error - 1 = + warnings - 2 = + info - 3 = + debug) */
	int LOG_LEVEL = 3;

	/** Application name */
	std::string APPLICATION_NAME = "WaterDropEngine";
	/** Application version */
	std::vector APPLICATION_VERSION_RAW {0, 2, 0}; // In order major, minor, patch
	uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(APPLICATION_VERSION_RAW[0], APPLICATION_VERSION_RAW[1], APPLICATION_VERSION_RAW[2]);
	std::string APPLICATION_VERSION_FORMATTED = "v" + std::to_string(APPLICATION_VERSION_RAW[0]) +  "."
	                                                         + std::to_string(APPLICATION_VERSION_RAW[1]) + "." + std::to_string(APPLICATION_VERSION_RAW[2]);

	/** Fixed initial CoreWindow width */
	int WIDTH = 1280;
	/** Fixed initial CoreWindow height */
	int HEIGHT = 720;

	/** Vulka, API version */
	uint32_t VULKAN_VERSION = VK_VERSION_1_2;
}
