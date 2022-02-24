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
			auto colorMatRed  = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {1.0f, 0.0f, 0.0}, VK_POLYGON_MODE_FILL);
			auto colorMatBlue = createMaterial<ColorMaterial>(std::pair<int, int>{0, 0}, Color {0.0f, 0.0f, 1.0}, VK_POLYGON_MODE_FILL);

			// MESH OBJECTS
			auto cubeMesh = createMesh<MeshLoader>("cube.obj");

			{
				WDE_PROFILE_SCOPE("examples::SceneExample02::gameObject1");
				// Game Object 1
				auto object3D = createGameObject("Cube Object 1");
				object3D->transform->position += glm::vec3(-5, 5, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatRed);
				meshRenderer->setMesh(cubeMesh);
			}
			{
				WDE_PROFILE_SCOPE("examples::SceneExample02::gameObject2");
				// Game Object 2
				auto object3D = createGameObject("Cube Object 2");
				object3D->transform->position += glm::vec3(0, 5, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatBlue);
				meshRenderer->setMesh(cubeMesh);
			}

			auto meshParent = createGameObject("Many 3D Objects Parent", true);
			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < 100; j++) {
					WDE_PROFILE_SCOPE("examples::SceneExample02::gameObject-i");
					auto object3D = createGameObject("Cube 3D Objects " + std::to_string(i), true);
					object3D->transform->setParent(meshParent->transform);
					object3D->transform->position += glm::vec3(+5.0 - (float) i * 5.0f, 0, 5.0f - (float) j * 5.0f);

					auto meshRenderer = object3D->addModule<MeshRendererModule>();
					meshRenderer->setMaterial(colorMatRed);
					meshRenderer->setMesh(cubeMesh);
				}
			}

			{
				WDE_PROFILE_SCOPE("examples::SceneExample02::gameObject3");
				// Game Object 3
				auto object3D = createGameObject("Cube Object 3");
				object3D->transform->position += glm::vec3(0, 0, 20);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatBlue);
				meshRenderer->setMesh(cubeMesh);
			}
			{
				WDE_PROFILE_SCOPE("examples::SceneExample02::gameObject4");
				// Game Object 4
				auto object3D = createGameObject("Cube Object 4");
				object3D->transform->position += glm::vec3(0, 0, -20);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(colorMatBlue);
				meshRenderer->setMesh(cubeMesh);
			}
		}
	};
}

