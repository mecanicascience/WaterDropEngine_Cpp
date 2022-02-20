#include "WdeInstance.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde {
	WdeInstance::WdeInstance() {
		// Create the engine instance
		WaterDropEngine::get();
	}

	void WdeInstance::startInstance() {
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
}
