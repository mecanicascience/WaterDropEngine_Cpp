#pragma once

#include "../../../wde.hpp"

namespace wde::render {
	/**
	 * Class that handle descriptor pools and allocate new pool when the last one is full.
	 */
	class DescriptorAllocator : public NonCopyable {
		public:
			/**
			 * Repartition of descriptor types in a pool.<br />
			 * Ex : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 2.0f => for 1000 descriptors allocated, 2000 will be uniform buffers.
			 */
			struct PoolSizes {
				std::vector<std::pair<VkDescriptorType,float>> sizes = {
						{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
						{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.0f },
						{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.0f },
						{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f },
						{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.0f },
						{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.0f },
						{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.0f },
						{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.0f },
						{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.0f },
						{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.0f },
						{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
				};
			};


			// Constructors
			explicit DescriptorAllocator() = default;
			~DescriptorAllocator() override;

			// Core functions
			/**
			 * Allocate a new descriptor set
			 * @param set A reference to the descriptor set to allocate
			 * @param layout The descriptor set layout that describe it's content
			 */
			void allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);
			/** Reset descriptor pools and add them to the free descriptor pools list */
			void resetPools();


		private:
			// Allocated pools
			std::vector<VkDescriptorPool> _usedPools;
			std::vector<VkDescriptorPool> _freePools;
			VkDescriptorPool _currentPool = VK_NULL_HANDLE;
			PoolSizes _descriptorSizes;

			// Core functions
			/** @return a new grabbed pool from the available free pools */
			VkDescriptorPool grabPool();
	};
}
