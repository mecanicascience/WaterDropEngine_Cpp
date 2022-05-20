#pragma once

#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>

#ifndef DEBUG
	// Engine mode production / debug
	// #define WDE_ENGINE_MODE_PRODUCTION
	#define WDE_ENGINE_MODE_DEBUG

	// GUI Status (auto-disabled if engine is in production mode)
	#define WDE_ACTIVATE_GUI
#endif


// AUTO (do not modify)
#ifndef LOG_LEVEL_VAR
	#ifdef WDE_ENGINE_MODE_DEBUG
		#define LOG_LEVEL 3 // LOG OUTPUT LEVEL (0 = error - 1 = + warnings - 2 = + info - 3 = + debug) - If Mode = Production, log_level <= 2
		#ifdef WDE_ACTIVATE_GUI
			#define WDE_GUI_ENABLED
		#endif
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
	extern bool IS_FULLSCREEN;

	extern uint32_t VULKAN_VERSION;

	// Scene data
	extern int MAX_SCENE_OBJECTS_COUNT;
	extern int MAX_GIZMO_OBJECTS_COUNT;

	// World config
	extern int CHUNK_SIZE;
	extern int MAX_CHUNKS_COUNT;
	extern int CHUNK_LOADED_DISTANCE;
}
#endif

