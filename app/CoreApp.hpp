#pragma once

#include <WaterDropEngine/includes.hpp>

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
