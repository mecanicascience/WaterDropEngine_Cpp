#pragma once

#include <WaterDropEngine/includes.hpp>

#include "render/CoreAppRenderer.hpp"
#include "scene/CoreAppScene.hpp"

using namespace wde;
class CoreApp {
	public:
		/** Create a new application */
		CoreApp() = default;

		/** Starts the application */
		void start();
};
