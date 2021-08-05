#pragma once

#include "../../src/WaterDropEngine/WdeScene/scenes/Scene.hpp"
#include "../../src/WaterDropEngine/WdeScene/objects/models/ModelLoader.hpp"

using namespace wde::scene;

class CoreAppScene : public Scene {
	public:
		// Core functions
		void initialize() override;
		void update() override;

	private:
		std::chrono::time_point<std::chrono::system_clock> _currentTime = std::chrono::high_resolution_clock::now();
};
