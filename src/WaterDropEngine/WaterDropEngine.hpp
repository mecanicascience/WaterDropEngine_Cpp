#pragma once

#include "../wde.hpp"
#include "WdeCore/Core/Core.hpp"

namespace wde {
	/**
	 * Main WDE Game Instance
	 */
	class WaterDropEngine {
		public:
			// Main instance
			/** Create a WDE main engine instance */
			static core::Core& get() {
				static auto *instance = new WaterDropEngine();
				return *instance->_instanceCore;
			}


		private:
			/** Core session instance */
			std::unique_ptr<core::Core> _instanceCore;

			/** Create the engine and starts it */
			explicit WaterDropEngine() {
				// Create logging service
				logger::LoggerHandler::initialize("logs/");
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
