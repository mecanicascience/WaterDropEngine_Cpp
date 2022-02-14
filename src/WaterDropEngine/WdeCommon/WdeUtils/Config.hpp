#pragma once

#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>

// Set engine mode (1 = production, 2 = debug)
#ifndef DEBUG
#define WDE_ENGINE_MODE 2
#endif

#ifndef WDE_CONFIG_DATA
namespace wde::Config {
	/** LOG OUTPUT LEVEL (0 = error - 1 = + warnings - 2 = + info - 3 = + debug) */
	extern int LOG_LEVEL;

	/** Application name */
	extern std::string APPLICATION_NAME;
	/** Application version */
	// extern std::vector APPLICATION_VERSION_RAW {1, 0, 0}; // In order major, minor, patch
	extern uint32_t APPLICATION_VERSION;
	extern std::string APPLICATION_VERSION_FORMATTED;

	/** Fixed initial CoreWindow width */
	extern int WIDTH;
	/** Fixed initial CoreWindow height */
	extern int HEIGHT;

	/** Current api vulkan version used */
	extern uint32_t VULKAN_VERSION;
}
#endif

