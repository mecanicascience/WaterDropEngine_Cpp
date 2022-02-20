#pragma once

#include "../../../wde.hpp"

namespace wde::render {
	/**
	 * Stores every descriptor created layouts as a hashmap to avoid duplicating them.
	 */
	class DescriptorLayoutCache {
		public:
			/**
			 * Stores infos of a descriptor set layout
			 */
			struct DescriptorLayoutInfo {
				std::vector<VkDescriptorSetLayoutBinding> bindings;
				bool operator==(const DescriptorLayoutInfo& other) const;
				size_t hash() const;
			};


			// Constructors
			explicit DescriptorLayoutCache() = default;
			~DescriptorLayoutCache();

			// Core functions
			/**
			 * Create a new descriptor set layout if it doesn't exist
			 * @param info Descriptor set informations
			 * @return The descriptor set layout
			 */
			VkDescriptorSetLayout createDescriptorLayout(VkDescriptorSetLayoutCreateInfo* info);


		private:
			/** Describes a descriptor layout set hash */
			struct DescriptorLayoutHash	{
				std::size_t operator()(const DescriptorLayoutInfo& k) const {
					return k.hash();
				}
			};

			std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> _layoutCache;
	};
}
