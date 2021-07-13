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
			glm::vec2 offset;
			alignas(16) glm::vec3 color;
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
