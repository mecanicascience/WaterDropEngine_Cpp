#pragma once

#include <WaterDropEngine/includes.hpp>
#include "../WaterApp/meshes/Plane.hpp"
#include "modules/WaterModule.hpp"
#include "materials/WaterMaterialCompute.hpp"

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
			int planeSizeFactor = 10;
			waterPlane.addModule<WaterModule>(
					planeSizeFactor,
					std::make_shared<Plane>(planeSizeFactor * 32, planeSizeFactor * 32),
					std::make_shared<WaterMaterialCompute>(RenderStage {0, 0}, VK_POLYGON_MODE_LINE));
			waterPlane.getModule<TransformModule>().scale *= 12.0f; // Plane will be X units long
			waterPlane.getModule<TransformModule>().scale.y /= 8.0f;
			waterPlane.getModule<TransformModule>().position.y = 0.3f;
		}
};

