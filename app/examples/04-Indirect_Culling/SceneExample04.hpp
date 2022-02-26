#pragma once

#include "../../../src/WaterDropEngine/WdeScene/WdeSceneInstance.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/ColorMaterial.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/ControllerModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/meshes/MeshLoader.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/TextureMaterial.hpp"

using namespace wde;
using namespace wde::scene;

namespace examples {
	class SceneExample04 : public WdeSceneInstance {
		void setup() override {
			// EDITOR CAMERA
			{
				WDE_PROFILE_SCOPE("examples::SceneExample02::editorCamera");
				auto camera = createGameObject("Editor Camera");
				auto camModule = camera->addModule<CameraModule>();
				camModule->setAsActive();
				camModule->setFarPlane(822.5f);
				camera->addModule<ControllerModule>();
				camera->transform->position = glm::vec3 {-32.67f, 45.34f, -150.79f};
				camera->transform->rotation = glm::vec3 {0.43f, 4.29f, 0.0f};
			}

			// VIEWING CAMERA
			{
				WDE_PROFILE_SCOPE("examples::SceneExample02::viewingCamera");
				auto camera = createGameObject("Viewing Camera");
				auto cameraModule = camera->addModule<CameraModule>();
				cameraModule->setFarPlane(822.5f);
				camera->addModule<ControllerModule>();
				camera->transform->position = glm::vec3 {-201.0f, 27.95f, -299.61f};
				camera->transform->rotation = glm::vec3 {0.51f, 5.11f, 0.0f};
			}


			// MATERIALS
			// Viking room
			auto vikingMat  = createMaterial<TextureMaterial>(std::pair<int, int>{0, 0}, "res/models/viking_room/viking_room.png", VK_POLYGON_MODE_FILL);
			auto vikingMesh = createMesh<MeshLoader>("viking_room/viking_room.obj");

			// Uniform material
			auto colorMatBlue = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {0.0f, 0.0f, 1.0}, VK_POLYGON_MODE_FILL);
			auto cubeMesh     = createMesh<MeshLoader>("cube.obj");


			// GAME OBJECTS
			{
				// Game Object 1
				auto object3D = createGameObject("Cube Object 1");
				object3D->transform->position += glm::vec3(-5, 5, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatBlue);
				meshRenderer->setMesh(cubeMesh);
			}

			auto meshParent = createGameObject("Many 3D Objects Parent", true);
			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < 100; j++) {
					auto object3D = createGameObject("Viking 3D Objects " + std::to_string(j + i * 100), true);
					object3D->transform->setParent(meshParent->transform);
					object3D->transform->position += glm::vec3(+5.0 - (float) i * 5.0f, 0, 5.0f - (float) j * 5.0f);
					object3D->transform->rotation = {3 * glm::half_pi<float>(), 0.5f, 0.0f};
					object3D->transform->scale *= 3.0f;

					auto meshRenderer = object3D->addModule<MeshRendererModule>();
					meshRenderer->setMaterial(vikingMat);
					meshRenderer->setMesh(vikingMesh);
				}
			}

			{
				// Game Object 3
				auto object3D = createGameObject("Cube Object 2");
				object3D->transform->position += glm::vec3(0, 0, 20);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatBlue);
				meshRenderer->setMesh(cubeMesh);
			}
		}
	};
}

