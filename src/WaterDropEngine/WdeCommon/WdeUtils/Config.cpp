#include "Config.hpp"

namespace wde::Config {
	// Core data
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

	/** Vulkan API version */
	uint32_t VULKAN_VERSION = VK_API_VERSION_1_2;


	// Scene config
	/** Max objects in the scene */
	int MAX_SCENE_OBJECTS_COUNT = 100000;
}
