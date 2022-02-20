#pragma once

#include "../../../src/WaterDropEngine/WdeScene/WdeSceneInstance.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/ColorMaterial.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/ControllerModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/meshes/MeshLoader.hpp"

using namespace wde;
using namespace wde::scene;

namespace examples {
	class SceneExample02 : public WdeSceneInstance {
		void setup() override {
			// Viewing camera
			auto camera = createGameObject("Viewing Camera");
			auto camModule = camera->addModule<CameraModule>();
			camModule->setAsActive();
			camera->addModule<ControllerModule>();
			camera->transform->position = glm::vec3 {0.0f, 0.0f, -6.0f};

			// Material and mesh
			auto colorMatRed = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {1.0f, 0.0f, 0.0}, VK_POLYGON_MODE_FILL);
			auto cubeMesh = createMesh<MeshLoader>("cube.obj");


			// Cube object
			auto object3D = createGameObject("Cube Object");
			object3D->transform->position += glm::vec3(0, 0, 0);

			auto meshRenderer = object3D->addModule<MeshRendererModule>();
			meshRenderer->setMaterial(colorMatRed);
			meshRenderer->setMesh(cubeMesh);
		}
	};
}

