#pragma once

#include "../WdeCommon/WdeError/WdeStatus.hpp"
#include "../WdeRenderingEngine/WdeRenderEngine.hpp"

namespace wde {
	class WdeInstance {
		public:
			WdeInstance() = default;
			~WdeInstance() = default;

			/**
			 * Initialize the WdeInstance and all of its important different class components
			 * @param logLevel The level of the displayed console log
			 * @param logActivatedChannels A vector containing a list of every enabled channels
			 */
			WdeStatus initialize(Logger::LoggerLogLevel logLevel, std::vector<LoggerChannel> logActivatedChannels);

			/** Run the WdeInstance and all of its important different class components */
			WdeStatus run();

			/** CleanUp the WdeInstance and all of its important different class components */
			WdeStatus cleanUp();


			// Getters and setters
			renderEngine::WdeRenderEngine& getWdeRenderingEngine() { return wdeRenderingEngine; };

	private:
			/** The main rendering engine */
			renderEngine::WdeRenderEngine wdeRenderingEngine {};
	};
}
