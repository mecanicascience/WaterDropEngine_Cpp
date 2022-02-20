#include "CommandBuffer.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	// Core functions
	CommandBuffer::CommandBuffer(bool begin, VkCommandBufferLevel bufferLevel) : _bufferLevel(bufferLevel) {
		WDE_PROFILE_FUNCTION();
		// Create allocation infos about buffer allocation
		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = WaterDropEngine::get().getRender().getInstance().getCommandPool()->getCommandPool();
		// Primary (can be submitted to queue but cannot be called from other command commands) or
		// Secondary (cannot be submitted directly but can be called from primary command commands) command buffer
		allocInfo.level = _bufferLevel;
		allocInfo.commandBufferCount = 1;

		// Allocate
		if (vkAllocateCommandBuffers(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &allocInfo, &_commandBuffer) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to allocate command commands.");

		// Begin recording
		if (begin)
			this->begin();
	}

	CommandBuffer::~CommandBuffer() {
		WDE_PROFILE_FUNCTION();
		CoreInstance& instance = WaterDropEngine::get().getRender().getInstance();
		// Cleanup command buffer
		vkFreeCommandBuffers(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), WaterDropEngine::get().getRender().getInstance().getCommandPool()->getCommandPool(), 1, &_commandBuffer);
	}


	// Command buffer functions
	void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
		WDE_PROFILE_FUNCTION();
		if (_running)
			return;

		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = flags;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(_commandBuffer, &beginInfo) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to begin recording command buffer.");

		_running = true;
	}

	void CommandBuffer::end() {
		WDE_PROFILE_FUNCTION();
		if (!_running)
			return;

		if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to record command buffer.");

		_running = false;
	}

	void CommandBuffer::submit(VkFence const &fence, VkSemaphore const &waitSemaphore, VkSemaphore const &signalSemaphore) {
		WDE_PROFILE_FUNCTION();
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

		// Make sure fence status is currently not signaled
		if (fence != VK_NULL_HANDLE)
			vkResetFences(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), 1, &fence);

		// Test success and submit queue (triggers fence when frame end submitting to given queue)
		if (vkQueueSubmit(getQueue(), 1, &submitInfo, fence) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to submit draw command buffer.");
	}

	void CommandBuffer::submitIdle() {
		end();
		submit();
		waitForQueueIdle();
	}



	// Helpers
	VkQueue CommandBuffer::getQueue() {
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice();
		// Submit to graphics queue
		return device.getGraphicsQueue();
	}

	void CommandBuffer::waitForQueueIdle() {
		vkQueueWaitIdle(getQueue());
	}
}
