#include "DescriptorPool.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	DescriptorPool::DescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags) {
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating a descriptor pool." << logger::endl;

		// Create pool
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = flags;
		poolInfo.maxSets = maxSets;
		poolInfo.poolSizeCount = (uint32_t) poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();

		if (vkCreateDescriptorPool(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &poolInfo, nullptr, &_pool) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create the descriptor pool.");
	}

	DescriptorPool::~DescriptorPool() {
		vkDestroyDescriptorPool(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _pool, nullptr);
	}
}
