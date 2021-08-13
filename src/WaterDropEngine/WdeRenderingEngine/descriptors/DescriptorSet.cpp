#include "DescriptorSet.hpp"
#include "../WdeRenderEngine.hpp"

namespace wde::renderEngine {
	// Constructor
	DescriptorSet::DescriptorSet(std::vector<DescriptorSetBindingData> bindingsData) : _bindingsData(std::move(bindingsData)) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Creating a descriptor set.", LoggerChannel::RENDERING_ENGINE);

		// Create layouts
		createBindingLayouts();
	}

	DescriptorSet::~DescriptorSet() {
		// Destroy set buffers
		for (auto& [bindingIndex, bufferStruct] : _bindingsBuffers) {
			vkDestroyBuffer(WdeRenderEngine::get().getSelectedDevice().getDevice(), bufferStruct.buffer, nullptr);
			vkFreeMemory(WdeRenderEngine::get().getSelectedDevice().getDevice(), bufferStruct.bufferMemory, nullptr);
		}

		// Destroy set layout
		_descriptorSetLayout = nullptr;
	}




	// Core functions
	void DescriptorSet::createBindingLayouts() {
		// Create binding layouts
		for (auto& bindingData : _bindingsData) {
			VkDescriptorSetLayoutBinding bindingLayout {};
			bindingLayout.binding = bindingData._bindingIndex;
			bindingLayout.descriptorCount = bindingData._descriptorCount;
			bindingLayout.descriptorType = bindingData._descriptorType;
			bindingLayout.stageFlags = bindingData._shaderStageFlags;
			bindingLayout.pImmutableSamplers = nullptr; // Optional

			_bindingsLayouts.push_back(bindingLayout);
		}
	}

	void DescriptorSet::createLayout(VkDescriptorSetLayout& descriptorSetLayout) {
		// Create descriptor layout
		VkDescriptorSetLayoutCreateInfo setLayoutInfo = {};
		setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setLayoutInfo.pNext = nullptr;
		setLayoutInfo.flags = 0;
		setLayoutInfo.bindingCount = static_cast<uint32_t>(_bindingsLayouts.size());
		setLayoutInfo.pBindings = _bindingsLayouts.data();

		if (vkCreateDescriptorSetLayout(WdeRenderEngine::get().getSelectedDevice().getDevice(), &setLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw WdeException("Failed to create a description set layout.", LoggerChannel::RENDERING_ENGINE);

		_descriptorSetLayout = &descriptorSetLayout;
	}



	void DescriptorSet::initialize() {
		WDE_PROFILE_FUNCTION();
		// Create bindings
		for (auto& binding : _bindingsData) {
			// Uniform buffers
			if (binding._descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				addBuffer((int) binding._bindingIndex, binding._elementSize);

			// Image sampler
			else if (binding._descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				addImageSampler((int) binding._bindingIndex, binding._imageView, binding._imageSampler);

			// Input attachment
			else if (binding._descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
				addInputAttachment((int) binding._bindingIndex, binding._renderPassIndex, binding._attachmentBindingIndex);

			// Not implemented
			else
				throw WdeException("Trying to create a descriptor binding type that has not currently been implemented.", LoggerChannel::RENDERING_ENGINE);
		}
	}

	void DescriptorSet::recreate() {
		// TODO
	}

	void DescriptorSet::updateBuffer(int bindingIndex, const void* newData) {
		if (_bindingsBuffers[bindingIndex].bufferMemory == nullptr) // If no buffer memory (ex camera object), return
			return;
		WDE_PROFILE_FUNCTION();

		void* data;
		vkMapMemory(WdeRenderEngine::get().getSelectedDevice().getDevice(), _bindingsBuffers[bindingIndex].bufferMemory,
					0, _bindingsBuffers[bindingIndex].bufferSize, 0, &data);
		memcpy(data, newData, _bindingsBuffers[bindingIndex].bufferSize);
		vkUnmapMemory(WdeRenderEngine::get().getSelectedDevice().getDevice(), _bindingsBuffers[bindingIndex].bufferMemory);
	}




	// Core functions
	void DescriptorSet::addBuffer(int bindingIndex, int bufferSize) {
		WDE_PROFILE_FUNCTION();
		// Create buffer struct
		_bindingsBuffers[bindingIndex] = DescriptorSetBindingBuffer {};
		_bindingsBuffers[bindingIndex].bindingIndex = bindingIndex;
		_bindingsBuffers[bindingIndex].bufferSize = bufferSize;

		// Create buffer
		BufferUtils::createBuffer(WdeRenderEngine::get().getSelectedDevice().getPhysicalDevice(),
								  WdeRenderEngine::get().getSelectedDevice().getDevice(),
								  bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								  _bindingsBuffers[bindingIndex].buffer, _bindingsBuffers[bindingIndex].bufferMemory);

		// Vulkan binding buffer
		VkDescriptorBufferInfo bufferInfo {};
		bufferInfo.buffer = _bindingsBuffers[bindingIndex].buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = bufferSize;

		// Write buffers into the set
		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;
		setWrite.dstBinding = bindingIndex;
		setWrite.dstArrayElement = 0; // if we write array, starting array index (default : 0)
		setWrite.dstSet = _descriptorSet;
		setWrite.descriptorCount = 1; // Update 1 descriptor at a time
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setWrite.pBufferInfo = &bufferInfo;
		setWrite.pImageInfo = nullptr;
		setWrite.pTexelBufferView = nullptr;

		// Update sets
		vkUpdateDescriptorSets(WdeRenderEngine::get().getSelectedDevice().getDevice(), 1, &setWrite, 0, nullptr);
	}

	void DescriptorSet::addImageSampler(int bindingIndex, VkImageView& imageView, VkSampler& imageSampler) {
		WDE_PROFILE_FUNCTION();
		// Create image info
		VkDescriptorImageInfo imageInfo {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = imageView;
		imageInfo.sampler = imageSampler;

		// Write buffers into the set
		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;
		setWrite.dstBinding = bindingIndex;
		setWrite.dstArrayElement = 0; // if we write array, starting array index (default : 0)
		setWrite.dstSet = _descriptorSet;
		setWrite.descriptorCount = 1; // Update 1 descriptor at a time
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setWrite.pBufferInfo = nullptr;
		setWrite.pImageInfo = &imageInfo;
		setWrite.pTexelBufferView = nullptr;

		// Update sets
		vkUpdateDescriptorSets(WdeRenderEngine::get().getSelectedDevice().getDevice(), 1, &setWrite, 0, nullptr);
	}


	void DescriptorSet::addInputAttachment(int bindingIndex, uint32_t renderPassIndex, uint32_t attachmentBindingIndex) {
		WDE_PROFILE_FUNCTION();

		// Get corresponding image view
		VkImageView& imageView = WdeRenderEngine::get().getRenderer()->getRenderPass(renderPassIndex)->getFrameBuffers().getImageAttachment(attachmentBindingIndex)->getView();

		// Create image info
		VkDescriptorImageInfo imageInfo {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = imageView;
		imageInfo.sampler = VK_NULL_HANDLE;

		// Write buffers into the set
		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;
		setWrite.dstBinding = bindingIndex;
		setWrite.dstArrayElement = 0; // if we write array, starting array index (default : 0)
		setWrite.dstSet = _descriptorSet;
		setWrite.descriptorCount = 1; // Update 1 descriptor at a time
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		setWrite.pBufferInfo = nullptr;
		setWrite.pImageInfo = &imageInfo;
		setWrite.pTexelBufferView = nullptr;

		// Update sets
		vkUpdateDescriptorSets(WdeRenderEngine::get().getSelectedDevice().getDevice(), 1, &setWrite, 0, nullptr);
	}
}


