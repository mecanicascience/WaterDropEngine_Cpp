#pragma once

#include "../../../wde.hpp"
#include "../core/CoreDeviceHelper.hpp"

namespace wde::renderEngine {
	/**
	 * The rendering engine command pool to allocate command commands from.
	 * Each command pool is associated with a thread.
	 */
	class CommandPool {
		public:
			// Core functions
			CommandPool(const std::thread::id &threadID);
			~CommandPool() = default;

			void cleanUp();

			// Getters and setters
			VkCommandPool& getCommandPool() { return _commandPool; }


		private:
			/** The binded thread ID of the command pool */
			const std::thread::id &_threadID;
			/** The associated command pool */
			VkCommandPool _commandPool;
	};
}
