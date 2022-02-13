#pragma once

#include <utility>

#include "../../../wde.hpp"

namespace wde::render {
	/**
	 * Describes a descriptor pool
	 */
	class DescriptorPool {
		public:
			/**
			 * Create a new descriptor pool
			 * @param poolSizes The different buffer sizes of the descriptor pool
			 * @param maxSets The max sets of the pool (default 8192, can change to 16384)
			 * @param initialize Should initialize the pool
			 */
			explicit DescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets = 8192, bool initialize = true);
			~DescriptorPool();


			// Core functions
			/**
			 * Create a new pool
			 * @param flags Default : No flags
			 */
			void createPool(VkDescriptorPoolCreateFlags flags = 0);


			// Getters and setters
			VkDescriptorPool& getPool() { return _pool; }


		private:
			// Core parameters
			std::vector<VkDescriptorPoolSize> _poolSizes;
			VkDescriptorPool _pool {};
			uint32_t _maxSets;
	};
}

