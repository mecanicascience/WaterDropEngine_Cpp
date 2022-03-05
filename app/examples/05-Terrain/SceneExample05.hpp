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
				camModule->setFarPlane(900.0f);
				camera->transform->position = glm::vec3 {0.0f, 0.0f, -6.0f};
			}


			// Terrain game object
			{
				auto terrainGO = createGameObject("Test Terrain");

				terrainGO->transform->position -= glm::vec3 {500, 0, 500};
				terrainGO->addModule<TerrainModule>("res/terrain/testTerrain/heightmap.png", 50, 1000, glm::vec4 {0, 0, 1000, 1000});
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

