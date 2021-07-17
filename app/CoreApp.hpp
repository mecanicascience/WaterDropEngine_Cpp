#pragma once

#include "../src/WaterDropEngine/WaterDropEngine.hpp"
#include "renderEngine/CoreAppPipeline.hpp"
#include "renderEngine/CoreAppRenderer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

using namespace wde;
class CoreApp {
public:
	// Setup push constants
	struct SimplePushConstantData {
		glm::mat2 transform {1.f}; // initialize with identity matrix
		glm::vec2 offset;
		alignas(16) glm::vec3 color; // (must be aligned as a 16 bytes value) as (x, y, -, -, r, g, b)
	};

	/** Create a new application */
	CoreApp() = default;

	/**
	 * Initialize the application
	 * @return An execution status value
	 */
	WdeStatus initialize();


private:
	WaterDropEngine engine {};
};
