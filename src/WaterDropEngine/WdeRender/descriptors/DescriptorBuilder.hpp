#pragma once

#include "../../../wde.hpp"
#include "DescriptorLayoutCache.hpp"
#include "DescriptorAllocator.hpp"

namespace wde::render {
	/**
	 * This class contains methods to create a new descriptor set.
	 */
	class DescriptorBuilder {
		public:
			// Core functions
			/** Starts the creation of a new descriptor set */
			static DescriptorBuilder begin();

			/**
			 * Bind a buffer to the set
			 * @param binding Index of the bounded buffer in the set
			 * @param bufferInfo
			 * @param type
			 * @param stageFlags
			 * @return The descriptor builder instance
			 */
			DescriptorBuilder& bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

			/**
			 * Bind an image to the set
			 * @param binding Index of the bounded buffer in the set
			 * @param imageInfo
			 * @param type
			 * @param stageFlags
			 * @return The descriptor builder instance
			 */
			DescriptorBuilder& bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);


			// Builder functions
			/**
			 * Build the descriptor set and layout
			 * @param set A reference to the descriptor set to create
			 * @param layout A reference to the descriptor set layout
			 */
			void build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);
			/**
			 * Build the descriptor set
			 * @param set A reference to the descriptor set to create
			 */
			void build(VkDescriptorSet& set);


		private:
			// Stores descriptor writes and bindings
			std::vector<VkWriteDescriptorSet> _writes {};
			std::vector<VkDescriptorSetLayoutBinding> _bindings {};

			// Reference to the corresponding allocations classes
			DescriptorLayoutCache* _cache;
			DescriptorAllocator* _alloc;
	};
}
