#pragma once

#include "../wde.hpp"
#include "WdeCore/Core/Core.hpp"

namespace wde {
	/**
	 * Main WDE Game Instance
	 */
	class WaterDropEngine {
		public:
			/** Create a WDE main engine instance */
			static core::Core& get() {
				static auto *instance = new WaterDropEngine();
				return *instance->_instanceCore;
			}


		private:
			/** core session instance */
			std::unique_ptr<core::Core> _instanceCore;


			/** Create the engine and starts it */
			explicit WaterDropEngine() : _instanceCore(std::make_unique<core::Core>()) {
				// Create logging service
				logger::LoggerHandler::initialize("logs/");
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Initializing program ========" << logger::endl;
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Initializing program." << logger::endl;
			}
	};
}
