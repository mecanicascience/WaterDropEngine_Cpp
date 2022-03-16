#pragma once

#include "../../../wde.hpp"
#include "../core/CoreDevice.hpp"

namespace wde::render {
	/**
	 * A command pool to allocate commands buffers from.
	 */
	class CommandPool : public NonCopyable {
		public:
			// Core functions
			explicit CommandPool(const std::thread::id &threadID);
			~CommandPool() override;

			// Getters and setters
			VkCommandPool& getCommandPool() { return _commandPool; }


		private:
			/** The bound thread ID of the command pool */
			const std::thread::id &_threadID;
			/** The associated command pool */
			VkCommandPool _commandPool = VK_NULL_HANDLE;
	};
}
