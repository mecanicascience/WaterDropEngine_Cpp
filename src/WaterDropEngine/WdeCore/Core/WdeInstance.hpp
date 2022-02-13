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
			void startInstance();
			/** Tick for the engine instance (called by WaterDropEngine) */
			void tickInstance();
			/** Clean up the engine instance (called by WaterDropEngine) */
			void cleanUpInstance() {
				// Destroy render pipeline
				_pipeline->cleanUp();
				_pipeline.reset();

				// Clean up instance
				cleanUp();
			}
			/** Called when window format changes (called by WaterDropEngine) */
			void onWindowResizedInstance() {
				_pipeline->onWindowResized();
			}


			// Getters and setters
			render::WdeRenderPipeline& getPipeline() { return *_pipeline; }



		protected:
			/** Change the engine rendering pipeline instance */
			void setRenderPipeline(std::unique_ptr<render::WdeRenderPipeline> pipeline) {
				WDE_PROFILE_FUNCTION();
				_pipeline = std::move(pipeline);

				// Create pipeline
				_pipeline->setup();
			}


		private:
			/** Engine rendering pipeline */
			std::unique_ptr<render::WdeRenderPipeline> _pipeline;
	};
}
