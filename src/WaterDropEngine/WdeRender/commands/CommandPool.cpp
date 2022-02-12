#include "CommandPool.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	CommandPool::CommandPool(const std::thread::id &threadID) : _threadID(threadID) {
		WDE_PROFILE_FUNCTION();
		CoreInstance& render = WaterDropEngine::get().getRender().getInstance();

		// Get supported queues
		QueueFamilyIndices queueFamilyIndices = render.getDevice().findQueueFamilies();

		// Create command pool
		VkCommandPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily; // Connect graphics queues and command buffer
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

		if (vkCreateCommandPool(render.getDevice().getDevice(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create command pool.");

		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Created the command pool of thread " << _threadID << "." << logger::endl;
	}

	CommandPool::~CommandPool() {
		WDE_PROFILE_FUNCTION();
		// Destroy command pool
		vkDestroyCommandPool(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _commandPool, nullptr);
	}
}
