#pragma once

#include "../../../src/WaterDropEngine/WdeScene/WdeSceneInstance.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/ColorMaterial.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/ControllerModule.hpp"

using namespace wde;
using namespace wde::scene;

namespace examples {
	class SceneExample02 : public WdeSceneInstance {
		void setup() override {
			// EDITOR CAMERA
			{
				auto camera = createGameObject("Editor Camera");
				auto camModule = camera->addModule<CameraModule>();
				camModule->setAsActive();
				camera->addModule<ControllerModule>();
				camera->transform->position = glm::vec3 {0.0f, 0.0f, -8.0f};
			}

			// VIEWING CAMERA
			{
				auto camera = createGameObject("Viewing Camera");
				camera->addModule<CameraModule>();
				camera->addModule<ControllerModule>();
				camera->transform->position = glm::vec3{-2.0f, 0.0f, -2.0f};
			}

			// MATERIALS
			auto colorMatRed   = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {1.0f, 0.0f, 0.0});
			auto colorMatGreen = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {0.0f, 1.0f, 0.0});

			// MESH OBJECTS
			auto testMesh = createMesh<Mesh>("Test Mesh");
			auto testMesh2 = createMesh<Mesh>("Test Mesh 2");

			{
				// Game Object 1
				auto object3D = createGameObject("Test Object 1");
				object3D->transform->position += glm::vec3(-5, 0, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatRed);
				meshRenderer->setMesh(testMesh);
			}
			{
				// Game Object 2
				auto object3D = createGameObject("Test Object 2");
				object3D->transform->position += glm::vec3(0, 0, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatRed);
				meshRenderer->setMesh(testMesh2);
			}

			auto meshParent = createGameObject("Many 3D Objects Parent", true);
			for (int i = 0; i < 1000; i++) {
				auto object3D = createGameObject("Many 3D Objects " + std::to_string(i), true);
				object3D->transform->setParent(meshParent->transform);
				object3D->transform->position += glm::vec3(+5.0 - ((float) i) / 10.0, 0, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatRed);
				meshRenderer->setMesh(testMesh);
			}
		}
	};
}

