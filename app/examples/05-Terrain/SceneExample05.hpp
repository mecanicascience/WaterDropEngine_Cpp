#pragma once

#include "../../../src/WaterDropEngine/WdeScene/WdeSceneInstance.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/ColorMaterial.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/ControllerModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/meshes/MeshLoader.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/TerrainModule.hpp"
#include "../../../src/WaterDropEngine/WdeRender/images/TextureCube.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/SkyboxMaterial.hpp"

using namespace wde;
using namespace wde::scene;

namespace examples {
	class SceneExample05 : public WdeSceneInstance {
		void setup() override {
			// Viewing camera
			{
				auto camera = createGameObject("Viewing Camera");
				auto camModule = camera->addModule<CameraModule>();

				camModule->setAsActive();
				camera->addModule<ControllerModule>();
				camModule->setFarPlane(900000.0f);
				camera->transform->position = glm::vec3 {0.0f, 0.0f, -6.0f};
			}


			// Terrain game object
			{
				auto terrainGO = createGameObject("Test Terrain");
				terrainGO->transform->scale = {1.0f, 21.0f, 1.0f};
				terrainGO->addModule<TerrainModule>("res/terrain/testTerrain/heightmap.png", 50*2.0, 1000, glm::vec4 {0, 0, 12000, 12000});
			}

			// Robot
			{
				auto robotMat  = createMaterial<TextureMaterial>(std::pair<int, int>{0, 0}, "res/models/robot/model_robot.png", VK_POLYGON_MODE_FILL);
				auto robotMesh = createMesh<MeshLoader>("robot/model_robot.obj");
				auto object3D = createGameObject("Robot");
				object3D->transform->position += glm::vec3(0, 5, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(robotMat);
				meshRenderer->setMesh(robotMesh);
			}

			// Fougere
			{
				auto fougereMat  = createMaterial<TextureMaterial>(std::pair<int, int>{0, 0}, "res/models/fougere/fougere.png", VK_POLYGON_MODE_FILL);
				auto fougereMesh = createMesh<MeshLoader>("fougere/fougere.obj");
				auto object3D = createGameObject("Fougere");
				object3D->transform->position += glm::vec3(10, 5, 0);

				auto meshRenderer = object3D->addModule<MeshRendererModule>();
				meshRenderer->setMaterial(fougereMat);
				meshRenderer->setMesh(fougereMesh);
			}


			// Skybox
			{
				auto skyboxMesh = createMesh<MeshLoader>("skybox/skybox.obj");
				auto skyboxMaterial = createMaterial<SkyboxMaterial>(std::pair<int, int>{0, 0}, "res/models/skybox/", "jpg");

				auto skyboxGO = createGameObject("Skybox", true);
				auto skyboxMeshRenderer = skyboxGO->addModule<MeshRendererModule>();
				skyboxMeshRenderer->setMaterial(skyboxMaterial);
				skyboxMeshRenderer->setMesh(skyboxMesh);
			}
		}
	};
}

