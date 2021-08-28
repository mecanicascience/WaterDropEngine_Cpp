#pragma once

#include <WaterDropEngine/includes.hpp>
#include "../WaterApp/meshes/Plane.hpp"
#include "modules/WaterModule.hpp"

using namespace wde::scene;

class ComputeAppScene : public Scene {
	public:
		explicit ComputeAppScene() : Scene("ComputeAppScene") {}

		void initialize() override {
			// Add Camera
			auto& camera = createGameObject("Camera");
			camera.addModule<TransformControllerModule>();
			camera.addModule<CameraModule>();
			camera.getModule<TransformModule>().position = {-3.88f, 1.82f, -6.87f};
			camera.getModule<TransformModule>().rotation = {0.4f, 0.17f, 0.0f};


			// Plane
			auto& waterPlane = createGameObject("Water Plane");
			int planeSize = 50;
			waterPlane.addModule<WaterModule>(
					std::make_shared<Plane>(planeSize, planeSize),
					std::make_shared<ColorMaterial>(RenderStage {0, 0}, VK_POLYGON_MODE_FILL));
			waterPlane.getModule<TransformModule>().scale *= 12.0f; // Plane will be X units long
			waterPlane.getModule<TransformModule>().position.y = 0.3f;
		}
};

