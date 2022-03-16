#include "DescriptorBuilder.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	// Core functions
	DescriptorBuilder DescriptorBuilder::begin() {
		WDE_PROFILE_FUNCTION();
		DescriptorBuilder builder {};
		builder._cache = &WaterDropEngine::get().getRender().getInstance().getDescriptorLayoutCache();
		builder._alloc = &WaterDropEngine::get().getRender().getInstance().getCurrentDescriptorAllocator();
		return builder;
	}

	DescriptorBuilder& DescriptorBuilder::bind_buffer(uint32_t binding, const Buffer& buffer, VkDescriptorType type, VkShaderStageFlags stageFlags) {
		WDE_PROFILE_FUNCTION();

		// Create the descriptor binding for the layout
		VkDescriptorSetLayoutBinding newBinding {};
		newBinding.descriptorCount = 1;
		newBinding.descriptorType = type;
		newBinding.pImmutableSamplers = nullptr;
		newBinding.stageFlags = stageFlags;
		newBinding.binding = binding;

		_bindings.push_back(newBinding);


		// Create the descriptor write
		VkWriteDescriptorSet newWrite {};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;
		newWrite.descriptorCount = 1;
		newWrite.descriptorType = type;
		newWrite.pBufferInfo = buffer.getBufferInfo();
		newWrite.dstBinding = binding;

		_writes.push_back(newWrite);

		// Return the builder
		return *this;
	}

	DescriptorBuilder& DescriptorBuilder::bind_image(uint32_t binding, const VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags) {
		WDE_PROFILE_FUNCTION();

		// Create the descriptor binding for the layout
		VkDescriptorSetLayoutBinding newBinding {};
		newBinding.descriptorCount = 1;
		newBinding.descriptorType = type;
		newBinding.pImmutableSamplers = nullptr;
		newBinding.stageFlags = stageFlags;
		newBinding.binding = binding;

		_bindings.push_back(newBinding);

		// Create the descriptor write
		VkWriteDescriptorSet newWrite {};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;
		newWrite.descriptorCount = 1;
		newWrite.descriptorType = type;
		newWrite.pImageInfo = imageInfo;
		newWrite.dstBinding = binding;

		_writes.push_back(newWrite);

		// Return the builder
		return *this;
	}


	// Builder functions
	void DescriptorBuilder::build(VkDescriptorSet &set, VkDescriptorSetLayout &layout) {
		// Build layout first
		VkDescriptorSetLayoutCreateInfo layoutInfo {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.pNext = nullptr;
		layoutInfo.pBindings = _bindings.data();
		layoutInfo.bindingCount = _bindings.size();
		layout = _cache->createDescriptorLayout(&layoutInfo);

		// Allocate descriptor or returns it if it already was created
		_alloc->allocate(&set, layout);

		// Write descriptor
		for (VkWriteDescriptorSet& w : _writes)
			w.dstSet = set;

		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();
		vkUpdateDescriptorSets(device, _writes.size(), _writes.data(), 0, nullptr);
	}

	void DescriptorBuilder::build(VkDescriptorSet &set) {
		VkDescriptorSetLayout layout;
		return build(set, layout);
	}
}
