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
	// Core data
	extern std::string APPLICATION_NAME;
	extern uint32_t APPLICATION_VERSION;
	extern std::string APPLICATION_VERSION_FORMATTED;

	extern int WIDTH;
	extern int HEIGHT;

	extern uint32_t VULKAN_VERSION;

	// Scene data
	extern int MAX_SCENE_OBJECTS_COUNT;
}
#endif

