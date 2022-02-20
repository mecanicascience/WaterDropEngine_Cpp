#include "DescriptorAllocator.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	// Constructors
	DescriptorAllocator::~DescriptorAllocator() {
		WDE_PROFILE_FUNCTION();
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		// Delete every pool held
		for (auto pool : _freePools)
			vkDestroyDescriptorPool(device, pool, nullptr);

		for (auto pool : _usedPools)
			vkDestroyDescriptorPool(device, pool, nullptr);
	}


	// Core functions
	void DescriptorAllocator::allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout) {
		WDE_PROFILE_FUNCTION();
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		// Initialize the currentPool handle if it's null
		if (_currentPool == VK_NULL_HANDLE){
			_currentPool = grabPool();
			_usedPools.push_back(_currentPool);
		}

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;

		allocInfo.pSetLayouts = &layout;
		allocInfo.descriptorPool = _currentPool;
		allocInfo.descriptorSetCount = 1;

		// Try to allocate the descriptor set
		VkResult allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);

		switch (allocResult) {
			case VK_SUCCESS:
				return;

			case VK_ERROR_FRAGMENTED_POOL:
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				// Reallocate pool
				break;

			default:
				throw WdeException(LogChannel::SCENE, "Failed to allocate a new pool");
		}

		// Allocate a new pool and retry
		_currentPool = grabPool();
		_usedPools.push_back(_currentPool);

		allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);

		// If it still fails then we have big issues
		if (allocResult == VK_SUCCESS)
			return;
		throw WdeException(LogChannel::SCENE, "Failed to allocate a new pool");
	}

	void DescriptorAllocator::resetPools() {
		WDE_PROFILE_FUNCTION();
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		// Reset used pools and add them to free pools
		for (auto pool : _usedPools){
			vkResetDescriptorPool(device, pool, 0);
			_freePools.push_back(pool);
		}

		// Clear used pools
		_usedPools.clear();

		// Reset current pool handle
		_currentPool = VK_NULL_HANDLE;
	}

	VkDescriptorPool DescriptorAllocator::grabPool() {
		WDE_PROFILE_FUNCTION();
		if (!_freePools.empty()) {
			// Grab free pool from the free list
			VkDescriptorPool pool = _freePools.back();
			_freePools.pop_back();
			return pool;
		}
		else {
			auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();
			int poolAllocationCount = 1000; // allocate 1000 pools

			// Allocate a new pool
			std::vector<VkDescriptorPoolSize> sizes;
			sizes.reserve(_descriptorSizes.sizes.size());
			for (auto sz : _descriptorSizes.sizes) {
				sizes.push_back({ sz.first, uint32_t(sz.second * poolAllocationCount) });
			}
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = 0;
			pool_info.maxSets = poolAllocationCount;
			pool_info.poolSizeCount = (uint32_t)sizes.size();
			pool_info.pPoolSizes = sizes.data();

			VkDescriptorPool descriptorPool;
			vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);

			return descriptorPool;
		}
	}
}
