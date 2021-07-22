#include "WdeInstance.hpp"

namespace wde {
	WdeStatus WdeInstance::initialize(Logger::LoggerLogLevel logLevel, std::vector<LoggerChannel> logActivatedChannels) {
		// Initialize Logging system
		Logger::initialize(logLevel, logActivatedChannels);

		// Initialize 3D Rendering engine
		wdeRenderingEngine.initialize();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::run() {
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
		// CleanUp 3D Rendering Engine
		wdeRenderingEngine.cleanUp();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}
}
