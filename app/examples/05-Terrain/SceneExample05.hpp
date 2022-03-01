#pragma once

#include "../../../src/WaterDropEngine/WdeScene/WdeSceneInstance.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/materials/ColorMaterial.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/ControllerModule.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/meshes/MeshLoader.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/TerrainModule.hpp"

using namespace wde;
using namespace wde::scene;

namespace examples {
	class SceneExample05 : public WdeSceneInstance {
		void setup() override {
			// Viewing camera
			auto camera = createGameObject("Viewing Camera");
			auto camModule = camera->addModule<CameraModule>();
			camModule->setAsActive();
			camera->addModule<ControllerModule>();
			camera->transform->position = glm::vec3 {0.0f, 0.0f, -6.0f};

			// Terrain game object
			auto terrainGO = createGameObject("Test Terrain");
			terrainGO->addModule<TerrainModule>();
		}
	};
}

