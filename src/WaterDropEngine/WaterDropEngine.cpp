#include "WaterDropEngine.hpp"
#include "WdeCommon/WdeError/WdeException.hpp"
#include "WdeCommon/WdeLogger/Logger.hpp"
#include "WdeCore/WdeInstance.hpp"


namespace wde {
	WdeStatus WaterDropEngine::initialize() {
		try {
			// Create WaterDropEngine instance
			WdeInstance instance {};

			// Initialize instance
			Logger::debug("======== Initializing program ========", LoggerChannel::MAIN);
			if (instance.initialize() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error initializing engine");
			Logger::debug("======== Initialization done ========\n\n", LoggerChannel::MAIN);

			// Starts instance
			Logger::debug("======== Starting program ========", LoggerChannel::MAIN);
			if (instance.run() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error running engine");
			Logger::debug("======== Program ended ========\n\n", LoggerChannel::MAIN);

			// CleanUp instance
			Logger::debug("======== Cleaning up ========", LoggerChannel::MAIN);
			if (instance.cleanUp() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error cleaning up engine");
			Logger::debug("======== End of program ========\n", LoggerChannel::MAIN);
		}
		catch (const WdeException &e) {
			Logger::err(e.what(), LoggerChannel::MAIN);
			return WdeStatus::WDE_ERROR;
		}

		return WdeStatus::WDE_SUCCESS;
	}
}
