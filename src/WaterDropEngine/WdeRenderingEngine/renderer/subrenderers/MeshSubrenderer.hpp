#pragma once

#include "../Subrenderer.hpp"
#include "../../../WdeScene/WdeSceneManager.hpp"
#include "../../pipelines/PipelineGraphics.hpp"
#include "../../../WdeScene/camera/Camera.hpp"
#include "../../../WdeInputEngine/InputMovementController.hpp"

namespace wde::renderEngine {
	/**
	 * The main mesh renderer, that handles objects and camera
	 */
	class MeshSubrenderer : public Subrenderer {
		public:
			// Constructors
			/**
			 * Create the mesh sub-renderer at the given stage
			 * @param stage
			 */
			explicit MeshSubrenderer(const RenderStage &stage);

			/** Clean up mesh sub-renderer */
			~MeshSubrenderer() override = default;


			// Core functions
			/** Renders the meshes to the command buffer */
			void render(CommandBuffer &commandBuffer) override;

		private:
			// TODO Temp pipeline
			PipelineGraphics _pipeline;

			// TODO temp camera objects
			scene::Camera _camera;
			scene::GameObject _cameraViewerObject {};
			inputManager::InputMovementController _cameraController {};

			std::chrono::time_point<std::chrono::system_clock> _currentTime = std::chrono::high_resolution_clock::now();
	};
}
