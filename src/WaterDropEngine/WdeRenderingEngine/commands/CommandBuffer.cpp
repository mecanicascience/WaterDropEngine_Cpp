#include "CommandBuffer.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
	// Core functions
	CommandBuffer::CommandBuffer(bool begin, VkCommandBufferLevel bufferLevel) {
		// Create allocation infos about buffer allocation
		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = CoreInstance::get().getCommandPool()->getCommandPool();
		// Primary (can be submit to queue but cannot be called from other command commands) or
		// Secondary (cannot be submit directly but can be called from primary command commands) command commands
		allocInfo.level = bufferLevel;
		allocInfo.commandBufferCount = 1;

		// Allocate
		if (vkAllocateCommandBuffers(CoreInstance::get().getSelectedDevice().getDevice(), &allocInfo, &_commandBuffer) != VK_SUCCESS) {
			throw WdeException("Failed to allocate command commands.", LoggerChannel::RENDERING_ENGINE);
		}

		// Begin recording
		if (begin)
			this->begin();
	}

	void CommandBuffer::cleanUp() {
		// Cleanup command buffer
		vkFreeCommandBuffers(CoreInstance::get().getSelectedDevice().getDevice(), CoreInstance::get().getCommandPool()->getCommandPool(), 1, &_commandBuffer);
	}


	// Command buffer functions
	void CommandBuffer::begin() {
		if (_running)
			return;

		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(_commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw WdeException("Failed to begin recording command buffer.", LoggerChannel::RENDERING_ENGINE);
		}

		_running = true;
	}

	void CommandBuffer::end() {
		if (!_running)
			return;

		if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS) {
			throw WdeException("Failed to record command buffer.", LoggerChannel::RENDERING_ENGINE);
		}

		_running = false;
	}

	void CommandBuffer::submit(VkFence fence, VkSemaphore const &waitSemaphore, VkSemaphore const &signalSemaphore) {
		if (_running)
			end();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffer;

		if (waitSemaphore != VK_NULL_HANDLE) {
			// Pipeline stages used to wait at for graphics queue submissions.
			static VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			submitInfo.pWaitDstStageMask = &submitPipelineStages;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &waitSemaphore;
		}

		if (signalSemaphore != VK_NULL_HANDLE) {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &signalSemaphore;
		}

		// Resets blocking infos
		if (fence != VK_NULL_HANDLE)
			vkResetFences(CoreInstance::get().getSelectedDevice().getDevice(), 1, &fence);

		// Test success and submit queue (triggers fence when frame end submitting to given queue)
		if (vkQueueSubmit(getQueue(), 1, &submitInfo, fence) != VK_SUCCESS) {
			throw WdeException("Failed to submit draw command buffer.", LoggerChannel::RENDERING_ENGINE);
		}
	}



	// Helpers
	VkQueue CommandBuffer::getQueue() {
		auto& device = CoreInstance::get().getSelectedDevice();

		// For now, we only have graphics and no compute queue
		return device.getGraphicsQueue();
	}
}