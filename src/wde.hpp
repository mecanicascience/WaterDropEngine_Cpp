#pragma once

// C++ basic modules
#include <unordered_map>
#include <utility>
#include <windows.h>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>

// Glm configuration
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// Glfw and Vulkan
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

// Main used classes
#include "WaterDropEngine/WdeCommon/WdeUtils/Config.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp"
#include "WaterDropEngine/WdeCommon/WdeException/WdeException.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp"

// Json
#include "nlohmann/json.hpp"
using json = nlohmann::json;

// ImGUI includes
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_glfw.h"
#include "../lib/imgui/backends/imgui_impl_vulkan.h"
#include "../lib/imgui/imgui_internal.h"
#include "../lib/IconFontCppHeaders/IconsFontAwesome5.h"
