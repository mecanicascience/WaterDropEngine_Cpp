#include "WaterDropEngine.hpp"

namespace wde {
	WaterDropEngine::WaterDropEngine() {
		try {
			WDE_PROFILE_BEGIN_SESSION("Initialization", "debug/profiler_init.json");
			Logger::forceLog("======== Initializing program ========", LoggerChannel::MAIN);

			// Create WaterDropEngine instance
			_instance = std::make_unique<WdeInstance>();
		}
		catch (const WdeException &e) {
			Logger::forceLog("", LoggerChannel::MAIN);
			Logger::err(e.what(), e.getChannel());
		}
	}

	WdeStatus WaterDropEngine::initialize() {
		try {
			// Log level
			auto logLevel = Logger::LoggerLogLevel::DEBUG;
			std::vector<LoggerChannel> activatedChannels = { LoggerChannel::MAIN, LoggerChannel::RENDERING_ENGINE, LoggerChannel::COMMON };

			// Initialize instance
			if (_instance->initialize(logLevel, activatedChannels) != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error initializing engine.", LoggerChannel::MAIN);

			// End of initialization
			Logger::debug("======== Initialization done ========\n\n", LoggerChannel::MAIN);
			WDE_PROFILE_END_SESSION();
		}
		catch (const WdeException &e) {
			Logger::forceLog("", LoggerChannel::MAIN);
			Logger::err(e.what(), e.getChannel());
			return WdeStatus::WDE_ERROR;
		}

		return WdeStatus::WDE_SUCCESS;
	}

	WdeStatus WaterDropEngine::run() {
		try {
			WDE_PROFILE_BEGIN_SESSION("Running", "debug/profiler_run.json");
			// Starts instance
			Logger::debug("======== Starting program ========", LoggerChannel::MAIN);
			if (_instance->run() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error running engine.", LoggerChannel::MAIN);
			Logger::debug("======== Program ended ========\n\n", LoggerChannel::MAIN);
			WDE_PROFILE_END_SESSION();



			WDE_PROFILE_BEGIN_SESSION("Cleaning", "debug/profiler_clean.json");
			// CleanUp instance
			Logger::debug("======== Cleaning up ========", LoggerChannel::MAIN);
			if (_instance->cleanUp() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error cleaning up engine.", LoggerChannel::MAIN);
			Logger::forceLog("======== End of program ========\n", LoggerChannel::MAIN);
			WDE_PROFILE_END_SESSION();
		}
		catch (const WdeException &e) {
			Logger::forceLog("", LoggerChannel::MAIN);
			Logger::err(e.what(), e.getChannel());
			return WdeStatus::WDE_ERROR;
		}

		return WdeStatus::WDE_SUCCESS;
	}
}
