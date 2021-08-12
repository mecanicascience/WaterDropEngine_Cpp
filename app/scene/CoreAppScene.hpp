#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde::scene;

class CoreAppScene : public Scene {
	public:
		// Core functions
		void initialize() override;
		void update() override;
};
