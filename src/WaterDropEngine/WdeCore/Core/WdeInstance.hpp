#pragma once

#include "../../WdeRender/commands/CommandBuffer.hpp"
#include "../../WdeRender/render/WdeRenderPipeline.hpp"

namespace wde {
	/**
	 * Represents an engine instance
	 */
	class WdeInstance {
		public:
			explicit WdeInstance();

			// Engine functions (called by WaterDropEngine)
			virtual void initialize() = 0;
			virtual void update() = 0;
			virtual void cleanUp() = 0;


			/** Start the engine */
			void start();
			/** Tick for the engine instance (called by WaterDropEngine) */
			void tick();



		protected:
			/** Change the engine rendering pipeline instance */
			void setRenderPipeline(std::unique_ptr<render::WdeRenderPipeline> pipeline) {
				_pipeline = std::move(pipeline);
			}


		private:
			/** Engine rendering pipeline */
			std::unique_ptr<render::WdeRenderPipeline> _pipeline;
	};
}
