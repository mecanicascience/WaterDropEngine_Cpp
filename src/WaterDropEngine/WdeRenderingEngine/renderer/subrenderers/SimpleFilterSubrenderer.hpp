#pragma once

#include "../Subrenderer.hpp"
#include "../../../WdeScene/WdeSceneManager.hpp"
#include "../../pipelines/PipelineGraphics.hpp"

namespace wde::renderEngine {
	/**
	 * The main mesh renderer, that renders scene objects
	 */
	class SimpleFilterSubrenderer : public Subrenderer {
	public:
		// Constructors
		/**
		 * Create the mesh sub-renderer at the given stage
		 * @param stage
		 */
		explicit SimpleFilterSubrenderer(const RenderStage &stage);

		/** Clean up mesh sub-renderer */
		~SimpleFilterSubrenderer() override = default;


		// Core functions
		/** Renders the meshes to the command buffer */
		void render(CommandBuffer &commandBuffer) override;
	};
}

