#pragma once

#include <utility>

#include "../../../wde.hpp"

namespace wde::renderEngine {
	/**
	 * Describes a descriptor pool
	 */
	class DescriptorPool {
		public:
			/**
			 * Create a new descriptor pool
			 * @param poolSizes The different buffer sizes of the descriptor pool
			 * @param maxSets The max sets of the pool (default 8192, can change to 16384)
			 */
			explicit DescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets = 8192) : _poolSizes(std::move(poolSizes)), _maxSets(maxSets) {
				Logger::debug("Creating a descriptor pool.", LoggerChannel::RENDERING_ENGINE);

				// Initialize pool
				createPool();
			}

			~DescriptorPool() {
				vkDestroyDescriptorPool(CoreInstance::get().getSelectedDevice().getDevice(), _pool, nullptr);
			}

			// Getters and setters
			VkDescriptorPool& getPool() { return _pool; }


		private:
			// Core parameters
			std::vector<VkDescriptorPoolSize> _poolSizes;
			VkDescriptorPool _pool {};
			uint32_t _maxSets;


			// Core functions
			void createPool() {
				// Create pool
				VkDescriptorPoolCreateInfo poolInfo = {};
				poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolInfo.flags = 0;
				poolInfo.maxSets = _maxSets;
				poolInfo.poolSizeCount = (uint32_t) _poolSizes.size();
				poolInfo.pPoolSizes = _poolSizes.data();

				if (vkCreateDescriptorPool(CoreInstance::get().getSelectedDevice().getDevice(), &poolInfo, nullptr, &_pool) != VK_SUCCESS)
					throw WdeException("Failed to create the descriptor pool.", LoggerChannel::RENDERING_ENGINE);
			}
	};
}
