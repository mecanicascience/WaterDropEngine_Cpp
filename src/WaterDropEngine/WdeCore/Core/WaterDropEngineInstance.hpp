#pragma once

#include "../../WdeRender/commands/CommandBuffer.hpp"

namespace wde {
	/**
	 * Represents an engine instance
	 */
	class WaterDropEngineInstance {
		public:
			explicit WaterDropEngineInstance();

			// Engine functions (called by WaterDropEngine)
			virtual void initialize() = 0;
			virtual void update() = 0;
			virtual void render(render::CommandBuffer &commandBuffer) = 0;
			virtual void cleanUp() = 0;


			/** Start the engine */
			void start();
			/** Performs rendering operations (called by WaterDropEngine) */
			void renderWDE();
	};
}
