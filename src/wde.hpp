#pragma once

// C++ basic modules
#include <iostream>
#include <unordered_map>
#include <utility>
#include <windows.h>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>

// ImGUI includes
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_glfw.h"
#include "../lib/imgui/backends/imgui_impl_vulkan.h"
#include "../lib/imgui/imgui_internal.h"

// Main used classes
#include "WaterDropEngine/WdeCommon/WdeUtils/Config.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp"
#include "WaterDropEngine/WdeCommon/WdeException/WdeException.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp"
