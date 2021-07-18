#include "WaterDropEngine.hpp"


namespace wde {
	void WaterDropEngine::setupRenderEngine(renderEngine::GraphicsPipeline& graphicsPipeline, renderEngine::Renderer& renderer) {
		this->graphicsPipeline = &graphicsPipeline;
		this->renderer = &renderer;
	}



	WdeStatus WaterDropEngine::initialize() {
		try {
			// Log level
			auto logLevel = Logger::LoggerLogLevel::DEBUG;
			std::vector<LoggerChannel> activatedChannels = { LoggerChannel::MAIN, LoggerChannel::RENDERING_ENGINE, LoggerChannel::COMMON };

			// Create WaterDropEngine instance
			WdeInstance instance {};

			// Initialize instance
			Logger::forceLog("======== Initializing program ========", LoggerChannel::MAIN);
			if (instance.initialize(logLevel, activatedChannels) != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error initializing engine", LoggerChannel::MAIN);

			// Setup render engine
			renderEngine::WdeRenderEngine& renderingEngine = instance.getWdeRenderingEngine();

			// Set the default graphics pipelines
			renderingEngine.setDeviceGraphicsPipeline(renderingEngine.getSelectedDevice(), *graphicsPipeline, *renderer);

			Logger::debug("======== Initialization done ========\n\n", LoggerChannel::MAIN);

			// Starts instance
			Logger::debug("======== Starting program ========", LoggerChannel::MAIN);
			if (instance.run() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error running engine.", LoggerChannel::MAIN);
			Logger::debug("======== Program ended ========\n\n", LoggerChannel::MAIN);

			// CleanUp instance
			Logger::debug("======== Cleaning up ========", LoggerChannel::MAIN);
			if (instance.cleanUp() != WdeStatus::WDE_SUCCESS)
				throw WdeException("Error cleaning up engine.", LoggerChannel::MAIN);
			Logger::forceLog("======== End of program ========\n", LoggerChannel::MAIN);
		}
		catch (const WdeException &e) {
			Logger::forceLog("", LoggerChannel::MAIN);
			Logger::err(e.what(), e.getChannel());
			return WdeStatus::WDE_ERROR;
		}

		return WdeStatus::WDE_SUCCESS;
	}
}
