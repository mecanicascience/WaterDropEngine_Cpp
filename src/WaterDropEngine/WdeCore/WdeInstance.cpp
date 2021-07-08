#include "WdeInstance.hpp"
#include "../WdeCommon/WdeLogger/Logger.hpp"
#include "../WdeCommon/WdeError/WdeException.hpp"

namespace wde {
	WdeStatus WdeInstance::initialize() {
		// Initialize Logging system
		auto logLevel = Logger::LoggerLogLevel::DEBUG;
		std::vector<LoggerChannel> activatedChannels = { LoggerChannel::MAIN, LoggerChannel::RENDERING_ENGINE, LoggerChannel::COMMON };

		Logger::initialize(logLevel, activatedChannels);


		// Initialize 3D Rendering engine
		if (wdeRenderingEngine.initialize() != WdeStatus::WDE_SUCCESS)
			throw WdeException("Failed to initialize the Rendering Engine.");

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::run() {
		// Main game loop
		while (!wdeRenderingEngine.shouldEnd()) { // Ask the rendering engine if we need to quit
			Logger::debug("====== Updating new frame. ======", LoggerChannel::MAIN);

			// Run
			if (wdeRenderingEngine.tick() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error while ticking for Rendering Engine.");

			Logger::debug("====== End of tick. ======\n\n", LoggerChannel::MAIN);
		}

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::cleanUp() {
		// CleanUp 3D Rendering Engine
		if (wdeRenderingEngine.cleanUp() != WdeStatus::WDE_SUCCESS)
			throw WdeException("Failed to clean up the Rendering Engine.");

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}
}
