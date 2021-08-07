#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

namespace wde::Constants {
	// Application configuration
	extern const std::string APPLICATION_NAME = "WaterDropEngine";
	extern const std::vector APPLICATION_VERSION_RAW {0, 2, 0}; // In order major, minor, patch
	extern const uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(APPLICATION_VERSION_RAW[0], APPLICATION_VERSION_RAW[1], APPLICATION_VERSION_RAW[2]);
	extern const std::string APPLICATION_VERSION_FORMATTED = "v" + std::to_string(APPLICATION_VERSION_RAW[0]) +  "."
			+ std::to_string(APPLICATION_VERSION_RAW[1]) + "." + std::to_string(APPLICATION_VERSION_RAW[2]);

	// Window configuration
	extern const int WINDOW_INITIAL_WIDTH = 1200;
	extern const int WINDOW_INITIAL_HEIGHT = 700;

	// Gui configuration
	extern const int GUI_CONSOLE_MAX_LINES = 200;
}
