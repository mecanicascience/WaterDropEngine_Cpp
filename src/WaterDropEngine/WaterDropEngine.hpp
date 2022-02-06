#pragma once

#include "../wde.hpp"
#include "WdeCore/Core/Core.hpp"

namespace wde {
	class WaterDropEngineConfig {
		public:
			int _logLevel = 0;
			bool _useProfiling = false;

			/**
			 * Setup the engine config
			 * @param logLevel Log level (0 = only error, 1 = warnings too, 2 = info too, 3 = debug too)
			 * @param useProfiling True = use profiling
			 */
			WaterDropEngineConfig(int logLevel, bool useProfiling)
				: _logLevel(logLevel), _useProfiling(useProfiling) {};
	};


	/**
	 * Main WDE Game Instance
	 */
	class WaterDropEngine {
		public:
			// Main instance
			/** Create a WDE main engine instance */
			static core::Core& get() {
				return get({0, false});
			}

			/** Create a WDE main engine instance */
			static core::Core& get(WaterDropEngineConfig config) {
				static auto *instance = new WaterDropEngine(config);
				return *instance->_instanceCore;
			}


		private:
			/** Core session instance */
			std::unique_ptr<core::Core> _instanceCore;
			/** Core config */
			WaterDropEngineConfig _config;

			/** Create the engine and starts it */
			explicit WaterDropEngine(WaterDropEngineConfig config) : _config(config) {
				// Create logging service
				logger::LoggerHandler::initialize("logs/", _config._logLevel);
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Initializing program ========" << logger::endl;
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Initializing program." << logger::endl;

				// Create and start core
				_instanceCore = std::make_unique<core::Core>();
				_instanceCore->start();

				// Clean up core and logger
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Cleaning up ========" << logger::endl;
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Closing program." << logger::endl;
				_instanceCore.reset();
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== End of program ========" << logger::endl << logger::endl;
				logger::LoggerHandler::cleanUp();
			};
	};
}
