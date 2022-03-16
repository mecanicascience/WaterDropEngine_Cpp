#pragma once

#include "../../WdeRender/commands/CommandBuffer.hpp"
#include "../../WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../WdeScene/WdeSceneInstance.hpp"

namespace wde {
	/**
	 * Represents an engine instance
	 */
	class WdeInstance : public NonCopyable {
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
			void cleanUpInstance();
			/** Called when window format changes (called by WaterDropEngine) */
			void onWindowResizedInstance() const { _pipeline->onWindowResized(); }


			// Getters and setters
			render::WdeRenderPipelineInstance& getPipeline() { return *_pipeline; }
			scene::WdeSceneInstance& getScene() { return *_scene; }
			std::shared_ptr<scene::WdeSceneInstance>& getScenePointer() { return _scene; }




		protected:
			/** Change the engine rendering pipeline instance */
			void setRenderPipeline(std::shared_ptr<render::WdeRenderPipelineInstance> pipeline) {
				WDE_PROFILE_FUNCTION();
				_pipeline = std::move(pipeline);
				_pipeline->setup();
			}

			/** Change the engine rendering scene instance */
			void setScene(std::shared_ptr<scene::WdeSceneInstance> scene) {
				WDE_PROFILE_FUNCTION();
				_scene = std::move(scene);
				_scene->setup();
			}


		private:
			/** Engine rendering pipeline */
			std::shared_ptr<render::WdeRenderPipelineInstance> _pipeline;
			/** Engine active scene */
			std::shared_ptr<scene::WdeSceneInstance> _scene;
	};
}
