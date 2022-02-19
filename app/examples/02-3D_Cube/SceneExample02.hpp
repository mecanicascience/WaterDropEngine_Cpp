#pragma once

#include "../../../src/WaterDropEngine/WdeScene/WdeSceneInstance.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/ColorMaterial.hpp"

using namespace wde;
using namespace wde::scene;

namespace examples {
	class SceneExample02 : public WdeSceneInstance {
		void setup() override {
			// CAMERA
			auto camera = createGameObject("Main Camera");
			camera->addModule<CameraModule>();
			setActiveCamera(camera);

			// MATERIALS
			auto colorMatRed   = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {1.0f, 0.0f, 0.0});
			auto colorMatGreen = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {0.0f, 1.0f, 0.0});

			// MESH OBJECTS
			auto meshParent = createGameObject("Mesh Parent");
			auto testMesh = createMesh<Mesh>("Test Mesh");
			{
				// Game Object 1
				auto object3D_1 = createGameObject("First 3D Object");
				object3D_1->transform->setParent(meshParent->transform);
				object3D_1->transform->position += glm::vec3(-5, 0, 0);

				auto meshRenderer_1 = object3D_1->addModule<MeshRendererModule>();
				meshRenderer_1->setMaterial(colorMatRed);
				meshRenderer_1->setMesh(testMesh);
			}
			{
				// Game Object 2
				auto object3D_2 = createGameObject("Second 3D Object");
				object3D_2->transform->setParent(meshParent->transform);
				object3D_2->transform->position += glm::vec3(+5, 0, 0);

				auto meshRenderer_2 = object3D_2->addModule<MeshRendererModule>();
				meshRenderer_2->setMaterial(colorMatGreen);
				meshRenderer_2->setMesh(testMesh);
			}
		}
	};
}

