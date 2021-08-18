#pragma once

#include <WaterDropEngine/includes.hpp>
#include "meshes/Plane.hpp"
#include "materials/WaterMaterial.hpp"
#include "materials/DepthMaterial.hpp"
#include "materials/NormalMaterial.hpp"

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


			auto& landTexture = createGameObject("Land");
			std::vector<std::shared_ptr<Material>> landMaterials {
				std::make_shared<DepthMaterial> (RenderStage {0, 0}),
				std::make_shared<NormalMaterial>(RenderStage {0, 1}),
				std::make_shared<TextureMaterial>(RenderStage {0, 2}, "land.png"),
				std::make_shared<TextureMaterial>(RenderStage {0, 3}, "land.png")
			};
			landTexture.addModule<ModelModule>(std::make_shared<ModelLoader>("land.obj"), landMaterials);

			landTexture.getModule<TransformModule>().rotation = {3 * glm::half_pi<float>(), 0.0f, 0.0f};
			landTexture.getModule<TransformModule>().scale *= 0.01f;


			// Plane
			auto& waterPlane = createGameObject("Water Plane");
			int planeSize = 50;
			waterPlane.addModule<ModelModule>(
					std::make_shared<Plane>(planeSize, planeSize),
					std::make_shared<WaterMaterial>(RenderStage {0, 4}, VK_POLYGON_MODE_FILL, 1, 2, 3));
			waterPlane.getModule<TransformModule>().scale *= 12.0f; // Plane will be X units long
			waterPlane.getModule<TransformModule>().position.y = 0.3f;
		}
};

