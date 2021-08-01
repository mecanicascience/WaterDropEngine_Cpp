#pragma once

#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/Subrenderer.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "../src/WaterDropEngine/WdeRenderingStructures/objects/Model.hpp"
#include "../src/WaterDropEngine/WdeRenderingStructures/objects/GameObject.hpp"
#include "../src/WaterDropEngine/WdeRenderingStructures/camera/Camera.hpp"
#include "../src/WaterDropEngine/WdeInputEngine/InputMovementController.hpp"

using namespace wde::renderEngine;
using namespace wde::renderStructures;

class CoreAppSubrenderer : public Subrenderer {
	public:
		/** Creates the renderer given the stage */
		explicit CoreAppSubrenderer(const RenderStage &stage);

		/** Records commands on the given command buffer */
		void render(CommandBuffer &commandBuffer) override;

		// Add a game object to the list
		void addGameObject(GameObject& gameObject) { _gameObjects.push_back(gameObject); }

	private:
		/** The rendering graphics pipeline */
		PipelineGraphics _pipeline;
		/** The rendered game objects */
		std::vector<GameObject> _gameObjects;

		Camera _camera;
		GameObject _cameraViewerObject {};
		InputMovementController _cameraController {};

		std::chrono::time_point<std::chrono::system_clock> _currentTime = std::chrono::high_resolution_clock::now();
};
