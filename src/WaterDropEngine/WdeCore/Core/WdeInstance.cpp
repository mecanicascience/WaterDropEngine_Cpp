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
		// Update the engine
		update();

		// Tick for the engine pipeline
		if (_pipeline == nullptr)
			throw WdeException(LogChannel::CORE, "The engine has no render pipeline.");
		_pipeline->tick();

		// Tick for the scene
		if (_scene != nullptr)
			_scene->tick();
	}
}
