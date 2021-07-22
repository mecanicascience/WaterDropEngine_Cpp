#include "CommandPool.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
	CommandPool::CommandPool(const std::thread::id &threadID) : _threadID(threadID) {
		// Get supported queues
		QueueFamilyIndices queueFamilyIndices = CoreDeviceHelper::findQueueFamilies(CoreInstance::get().getSelectedDevice().getPhysicalDevice());

		// Create command pool
		VkCommandPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily; // Connect graphics queues and command buffer
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

		if (vkCreateCommandPool(CoreInstance::get().getSelectedDevice().getDevice(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
			throw WdeException("Failed to create command pool.", LoggerChannel::RENDERING_ENGINE);
		}

		std::stringstream buffer;
		buffer << "Created the command pool of thread " << _threadID << ".";
		Logger::debug(buffer.str(), LoggerChannel::RENDERING_ENGINE);
	}

	void CommandPool::cleanUp() {
		// Destroy command pool
		vkDestroyCommandPool(CoreInstance::get().getSelectedDevice().getDevice(), _commandPool, nullptr);
	}
}
