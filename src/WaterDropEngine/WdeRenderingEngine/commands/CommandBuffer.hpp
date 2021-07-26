#pragma once

#include "../../../wde.hpp"

namespace wde::renderEngine {
	/**
	 * Class that represents a command buffer.
	 */
	class CommandBuffer {
		public:
			// Core functions
			/**
			 * Creates a new command buffer.
			 * @param begin If true, the program will start recording the command buffer
			 * @param bufferLevel The level of the command buffer (Primary or Secondary) - Default is primary
			 */
            explicit CommandBuffer(bool begin, VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			~CommandBuffer() = default;

			void initialize(bool begin);
			void cleanUp();


			// Command buffer functions
			/** Begins the recording state for this command buffer. */
			void begin();
			/** Ends the recording state for this command buffer. */
			void end();
			/** Submits the current command buffer to the specified queue */
			void submit(VkFence fence, const VkSemaphore &waitSemaphore, const VkSemaphore &signalSemaphore);


			// Getters and setters
			operator const VkCommandBuffer &() const { return _commandBuffer; }
			bool isRunning() { return _running; }


		private:
			/** True if the command buffer is running */
			bool _running = false;
			/** The buffer level */
			VkCommandBufferLevel _bufferLevel;

			/** The corresponding command buffer */
			VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;


			// Helpers
			static VkQueue getQueue();
	};
}
