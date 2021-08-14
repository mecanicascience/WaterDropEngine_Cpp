#pragma once

#include <WaterDropEngine/includes.hpp>
#include "meshes/Plane.hpp"
#include "materials/WaterMaterial.hpp"

using namespace wde::scene;

class WaterAppScene : public Scene {
	public:
		explicit WaterAppScene() : Scene("WaterAppScene") {}

		void initialize() override {
			// Add Camera
			auto& camera = createGameObject("Camera");
			camera.addModule<TransformControllerModule>();
			camera.addModule<CameraModule>();
			camera.getModule<TransformModule>().position = {-3.88f, 1.82f, -6.87f};
			camera.getModule<TransformModule>().rotation = {0.4f, 0.17f, 0.0f};

			// Plane
			auto& waterPlane = createGameObject("Water Plane");
			waterPlane.addModule<ModelModule>(
					std::make_shared<Plane>(30, 30),
					std::make_shared<WaterMaterial>(RenderStage {0, 0}, VK_POLYGON_MODE_FILL, 1));
			waterPlane.getModule<TransformModule>().scale *= 12.0f; // Plane will be X units long
			waterPlane.getModule<TransformModule>().position.y = 0.3f;


			// Land
			auto& land = createGameObject("Land");
			land.addModule<ModelModule>(
					std::make_shared<ModelLoader>("land.obj"),
					std::make_shared<TextureMaterial>(RenderStage {0, 1}, "land.png"));
			land.getModule<TransformModule>().rotation = {3*glm::half_pi<float>(), 0.0f, 0.0f};
			land.getModule<TransformModule>().scale *= 0.01f;
		}
};

