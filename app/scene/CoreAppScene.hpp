#pragma once

#include "../../src/WaterDropEngine/WdeScene/scenes/Scene.hpp"
#include "../../src/WaterDropEngine/WdeScene/objects/models/ModelLoader.hpp"
#include "../../src/WaterDropEngine/WdeScene/objects/models/ModelCube.hpp"
#include "../../src/WaterDropEngine/WdeScene/modules/TransformModule.hpp"
#include "../../src/WaterDropEngine/WdeScene/modules/ModelModule.hpp"

using namespace wde::scene;

class CoreAppScene : public Scene {
	public:
		// Core functions
		void initialize() override;
		void update() override;
};
