#include "WdeInstance.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde {
	WdeInstance::WdeInstance() {
		WDE_PROFILE_FUNCTION();
		// Create the engine instance
		WaterDropEngine::get();
	}

	void WdeInstance::startInstance() {
		WDE_PROFILE_FUNCTION();
		// Start the engine
		WaterDropEngine::get().start(*this);
	}

	void WdeInstance::tickInstance() {
		WDE_PROFILE_FUNCTION();
		// Check if there is scene and pipeline
		if (_scene == nullptr)
			throw WdeException(LogChannel::CORE, "The engine has no scene.");
		if (_pipeline == nullptr)
			throw WdeException(LogChannel::CORE, "The engine has no render pipeline.");

		// Update the engine
		update();

		// Tick for the engine pipeline
		_pipeline->tick();

		// Tick for the scene
		_scene->tick();
	}

	void WdeInstance::cleanUpInstance() {
		WDE_PROFILE_FUNCTION();
		// Destroy render scene
		if (_scene != nullptr) {
			_scene->cleanUp();
			_scene.reset();
		}

		// Destroy render pipeline
		_pipeline->cleanUp();
		_pipeline.reset();

		// Clean up instance
		cleanUp();
	}





	void WdeInstance::setRenderPipeline(std::shared_ptr<render::WdeRenderPipelineInstance> pipeline) {
		WDE_PROFILE_FUNCTION();
		_pipeline = std::move(pipeline);
		_pipeline->setup();
	}

	void WdeInstance::setScene(std::shared_ptr<scene::WdeSceneInstance> scene) {
		_scene = std::move(scene);
	}
}
