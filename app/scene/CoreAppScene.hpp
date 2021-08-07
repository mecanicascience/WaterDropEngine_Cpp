#pragma once

#include "../../src/WaterDropEngine/WdeScene/Scene.hpp"
#include "../../src/WaterDropEngine/WdeScene/objects/models/ModelLoader.hpp"
#include "../../src/WaterDropEngine/WdeScene/objects/models/ModelCube.hpp"
#include "../../src/WaterDropEngine/WdeScene/modules/TransformModule.hpp"
#include "../../src/WaterDropEngine/WdeScene/modules/ModelModule.hpp"
#include "../../src/WaterDropEngine/WdeScene/modules/CameraModule.hpp"
#include "../../src/WaterDropEngine/WdeScene/modules/TransformControllerModule.hpp"

using namespace wde::scene;

class CoreAppScene : public Scene {
	public:
		// Core functions
		void initialize() override;
		void update() override;
};
