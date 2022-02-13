#pragma once

#include "../commands/CommandBuffer.hpp"

namespace wde::render {
	/**
	 * Represents an instance of the engine rendering pipeline
	 */
	class WdeRenderPipeline {
		public:
			/** Create the pipeline */
			WdeRenderPipeline() = default;

			// Core WDE functions
			/** Tick for pipeline (called by the WdeInstance) */
			void tick();


		protected:
			/**
			 * Render the pipeline to a command buffer
			 * @param commandBuffer
			 */
			virtual void render(CommandBuffer& commandBuffer) = 0;
	};
}
