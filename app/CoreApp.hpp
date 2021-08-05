#pragma once

#include "../src/WaterDropEngine/WaterDropEngine.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.hpp"
#include "../src/WaterDropEngine/WdeScene/WdeSceneManager.hpp"

#include "render/CoreAppRenderer.hpp"
#include "scene/CoreAppScene.hpp"

using namespace wde;
class CoreApp {
	public:
		/** Create a new application */
		CoreApp() = default;

		/**
		 * Initialize the application
		 * @return An execution status value
		 */
		WdeStatus initialize();
};
