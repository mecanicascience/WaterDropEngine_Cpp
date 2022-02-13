#include "WdeInstance.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde {
	WdeInstance::WdeInstance() {
		// Create the engine instance
		WaterDropEngine::get();
	}

	void WdeInstance::start() {
		// Start the engine
		WaterDropEngine::get().start(*this);
	}

	void WdeInstance::tick() {
		// Update the engine
		update();

		// Tick for the engine pipeline
		if (_pipeline == nullptr)
			throw WdeException(LogChannel::CORE, "The engine has no render pipeline.");
		_pipeline->tick();
	}
}
