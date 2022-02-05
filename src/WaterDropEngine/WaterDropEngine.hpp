#pragma once

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

			/** Initialize the modules */
			explicit WaterDropEngine() {
				// Create core
				_instanceCore = std::make_unique<core::Core>();

				// Start core
				_instanceCore->start();

				// Clean up core
				_instanceCore.reset();
			}
	};
}
