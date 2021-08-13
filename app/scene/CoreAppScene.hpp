#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde::scene;

class CoreAppScene : public Scene {
	public:
		explicit CoreAppScene() : Scene("CoreAppScene") {}

		// Core functions
		void initialize() override;
};
