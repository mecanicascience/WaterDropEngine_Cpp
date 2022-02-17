#include "DescriptorPool.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	DescriptorPool::DescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags)
			: _poolSizes(std::move(poolSizes)), _maxSets(maxSets) {
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating a descriptor pool." << logger::endl;

		// Create pool
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = flags;
		poolInfo.maxSets = _maxSets;
		poolInfo.poolSizeCount = (uint32_t) _poolSizes.size();
		poolInfo.pPoolSizes = _poolSizes.data();

		if (vkCreateDescriptorPool(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &poolInfo, nullptr, &_pool) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create the descriptor pool.");
	}

	DescriptorPool::~DescriptorPool() {
		vkDestroyDescriptorPool(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _pool, nullptr);
	}
}
