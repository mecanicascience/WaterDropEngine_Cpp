#include "Config.hpp"

namespace wde::Config {
	// Core data
	/** Application name */
	std::string APPLICATION_NAME = "WaterDropEngine";
	/** Application version */
	std::vector APPLICATION_VERSION_RAW {1, 0, 0}; // In order major, minor, patch
	uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(APPLICATION_VERSION_RAW[0], APPLICATION_VERSION_RAW[1], APPLICATION_VERSION_RAW[2]);
	std::string APPLICATION_VERSION_FORMATTED = "v" + std::to_string(APPLICATION_VERSION_RAW[0]) +  "."
	                                                         + std::to_string(APPLICATION_VERSION_RAW[1]) + "." + std::to_string(APPLICATION_VERSION_RAW[2]);

	/** Fixed initial CoreWindow width */
	int WIDTH = 1280;
	/** Fixed initial CoreWindow height */
	int HEIGHT = 720;
	/** True if the windows is in fullscreen mode */
	bool IS_FULLSCREEN = false;

	/** Vulkan API version */
	uint32_t VULKAN_VERSION = VK_API_VERSION_1_2;


	// Scene config
	/** Max objects in each chunks */
	int MAX_CHUNK_OBJECTS_COUNT = 10000;
	/** Max objects in the gizmo scene */
	int MAX_GIZMO_OBJECTS_COUNT = 10000;


	// World config
	/** Size of a chunk */
	int CHUNK_SIZE = 126;
	/** Max chunks active in the scene */
	int MAX_CHUNKS_COUNT = 10000;
	/** Radius of the loaded chunks */
	int CHUNK_LOADED_DISTANCE = 1;
}
