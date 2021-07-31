#pragma once

// Use profiling
#define WDE_USE_PROFILING

// Glm configuration
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// Includes utils objects
#include "WaterDropEngine/WdeCommon/WdeError/WdeStatus.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp"
#include "WaterDropEngine/WdeCommon/WdeError/WdeException.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp"
#include "WaterDropEngine/WdeCommon/WdeUtils/NonCopyable.hpp"
#include "WaterDropEngine/WdeCommon/WdeUtils/Constants.hpp"
