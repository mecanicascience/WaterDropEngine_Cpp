#pragma once

#include "Gizmo.hpp"
#include "../../WdeRenderingEngine/commands/CommandBuffer.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"

namespace wde::scene {
	/**
	 * Class that will render every objects in the gizmo instance
	 */
	class GizmoRenderer {
		public:
			/**
			 * Create a new Gizmo renderer (this will create adequate pipelines)
			 * @param stage The rendering stage for pipelines
			 */
			explicit GizmoRenderer(RenderStage stage);


			/**
			 * Renders gizmo instance objects to the command buffer
			 * @param commandBuffer
			 * @param gizmoInstance
			 */
			void renderGizmoInstance(CommandBuffer &commandBuffer, Gizmo &gizmoInstance);


			// Helper functions
			/**
			 * @param position
			 * @param rotation
			 * @param scale
			 * @return The transform corresponding matrix given the position, rotation and scale
			 */
			glm::mat4 getTransform(glm::vec3 position, glm::vec3 rotation = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f}) const;


		private:
			/** Pipeline that will render separated lines as format (v1, v2) - (v3, v4) - .. */
			PipelineGraphics _pipelineSeparatedLines;
	};
}
