#pragma once

#include "../../src/WaterDropEngine/WdeScene/scenes/Scene.hpp"

using namespace wde::scene;

class CoreAppScene : public Scene {
	public:
		// Constructors
		explicit CoreAppScene();
		~CoreAppScene() override = default;

		// Core functions
		void initialize() override;
		void update() override;
		void render() override;
		void cleanUp() override;
};
