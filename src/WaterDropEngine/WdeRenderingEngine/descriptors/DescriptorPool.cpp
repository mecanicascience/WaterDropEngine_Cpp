#include "DescriptorPool.hpp"
#include "../WdeRenderEngine.hpp"

namespace wde::renderEngine {
	DescriptorPool::DescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets, bool initialize) : _poolSizes(std::move(poolSizes)), _maxSets(maxSets) {
		Logger::debug("Creating a descriptor pool.", LoggerChannel::RENDERING_ENGINE);

		// Initialize pool
		if (initialize)
			createPool();
	}

	DescriptorPool::~DescriptorPool() {
		vkDestroyDescriptorPool(WdeRenderEngine::get().getSelectedDevice().getDevice(), _pool, nullptr);
	}


	// Core functions
	/**
	 * Create a new pool
	 * @param flags Default : No flags
	 */
	void DescriptorPool::createPool(VkDescriptorPoolCreateFlags flags) {
		// Create pool
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = flags;
		poolInfo.maxSets = _maxSets;
		poolInfo.poolSizeCount = (uint32_t) _poolSizes.size();
		poolInfo.pPoolSizes = _poolSizes.data();

		if (vkCreateDescriptorPool(WdeRenderEngine::get().getSelectedDevice().getDevice(), &poolInfo, nullptr, &_pool) != VK_SUCCESS)
			throw WdeException("Failed to create the descriptor pool.", LoggerChannel::RENDERING_ENGINE);
	}
}

