#include "WdeInstance.hpp"

namespace wde {
	WdeStatus WdeInstance::initialize(Logger::LoggerLogLevel logLevel, std::vector<LoggerChannel> logActivatedChannels) {
		// Initialize Logging system
		Logger::initialize(logLevel, logActivatedChannels);


		// Initialize 3D Rendering engine
		if (wdeRenderingEngine.initialize() != WdeStatus::WDE_SUCCESS)
			throw WdeException("Failed to initialize the Rendering Engine.", LoggerChannel::MAIN);

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::run() {
		// Main game loop
		while (!wdeRenderingEngine.shouldEnd()) { // Ask the renderer engine if we need to quit
			Logger::debug("====== Updating new frame. ======", LoggerChannel::MAIN);

			// Run
			if (wdeRenderingEngine.tick() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error while ticking for Rendering Engine.", LoggerChannel::MAIN);

			Logger::debug("====== End of tick. ======\n\n", LoggerChannel::MAIN);
		}

		// Wait until every used device are ready
		auto devices = wdeRenderingEngine.getInstance().getDevices();
		for (auto device : devices) {
			vkDeviceWaitIdle(device.getDevice());
		}

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::cleanUp() {
		// CleanUp 3D Rendering Engine
		if (wdeRenderingEngine.cleanUp() != WdeStatus::WDE_SUCCESS)
			throw WdeException("Failed to clean up the Rendering Engine.", LoggerChannel::MAIN);

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}
}
