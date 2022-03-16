#pragma once

#include <utility>

#include "../../../wde.hpp"

namespace wde::render {
	/**
	 * Describes a descriptor pool
	 */
	class DescriptorPool : public NonCopyable {
		public:
			/**
			 * Create a new descriptor pool
			 * @param poolSizes The different buffer sizes of the descriptor pool
			 * @param maxSets The max sets of the pool (default 8192, can change to 16384)
			 * @param flags Flags applied at the pool creation
			 */
			explicit DescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets = 8192, VkDescriptorPoolCreateFlags flags = 0);
			~DescriptorPool() override;


			// Getters and setters
			VkDescriptorPool& getPool() { return _pool; }


		private:
			// Core parameters
			VkDescriptorPool _pool {};
	};
}

