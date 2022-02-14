#pragma once

#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>

// Set engine mode production / debug
#ifndef DEBUG
	// #define WDE_ENGINE_MODE_PRODUCTION
	#define WDE_ENGINE_MODE_DEBUG
#endif

#ifndef LOG_LEVEL_VAR
	#ifdef WDE_ENGINE_MODE_DEBUG
		#define LOG_LEVEL 3 // LOG OUTPUT LEVEL (0 = error - 1 = + warnings - 2 = + info - 3 = + debug) - If Mode = Production, log_level <= 2
	#else
		#define LOG_LEVEL 0
	#endif
#endif

#ifndef WDE_CONFIG_DATA
namespace wde::Config {
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

