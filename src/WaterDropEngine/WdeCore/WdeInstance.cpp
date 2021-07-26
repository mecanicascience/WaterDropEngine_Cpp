#include "WdeInstance.hpp"

namespace wde {
	WdeStatus WdeInstance::initialize() {
        WDE_PROFILE_FUNCTION();
		// Initialize 3D Rendering engine
		wdeRenderingEngine.initialize();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::run() {
        WDE_PROFILE_FUNCTION();
		// Main game loop
		while (!wdeRenderingEngine.shouldEnd()) { // Ask the renderer engine if we need to quit
			Logger::debug("====== Updating new frame. ======", LoggerChannel::MAIN);

			// Run
			wdeRenderingEngine.tick();

			Logger::debug("====== End of tick. ======\n\n", LoggerChannel::MAIN);
		}

		// Wait until every used device are ready
		wdeRenderingEngine.waitForDevicesReady();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::cleanUp() {
	    WDE_PROFILE_FUNCTION();
		// CleanUp 3D Rendering Engine
		wdeRenderingEngine.cleanUp();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}
}
