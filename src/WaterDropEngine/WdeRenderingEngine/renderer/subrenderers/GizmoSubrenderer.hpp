#pragma once

#include "../Subrenderer.hpp"
#include "../../../WdeScene/WdeSceneManager.hpp"
#include "../../pipelines/PipelineGraphics.hpp"

namespace wde::renderEngine {
	/**
	 * The main Gizmo renderer, that renders scene Gizmo objects
	 */
	class GizmoSubrenderer : public Subrenderer {
		public:
			// Constructors
			/**
			 * Create the mesh sub-renderer at the given stage
			 * @param stage
			 */
			explicit GizmoSubrenderer(const RenderStage &stage);

			/** Clean up mesh sub-renderer */
			~GizmoSubrenderer() override = default;


			// Core functions
			/** Renders the gizmo meshes to the command buffer */
			void render(CommandBuffer &commandBuffer) override;
	};
}
