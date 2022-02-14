#include "DescriptorSet.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	// Constructor
	DescriptorSet::DescriptorSet(std::vector<DescriptorSetBindingData> bindingsData) : _bindingsData(std::move(bindingsData)) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating a descriptor set." << logger::endl;

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

	DescriptorSet::~DescriptorSet() {
		// Destroy set buffers
		_bindingsBuffers.clear();

		// Destroy set layout
		_descriptorSetLayout = nullptr;
	}




	// Core functions
	void DescriptorSet::createLayout(VkDescriptorSetLayout& descriptorSetLayout) {
		// Create descriptor layout
		VkDescriptorSetLayoutCreateInfo setLayoutInfo = {};
		setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setLayoutInfo.pNext = nullptr;
		setLayoutInfo.flags = 0;
		if (_bindingsLayouts.empty()) {
			setLayoutInfo.bindingCount = 0;
			setLayoutInfo.pBindings = nullptr;
		}
		else {
			setLayoutInfo.bindingCount = static_cast<uint32_t>(_bindingsLayouts.size());
			setLayoutInfo.pBindings = _bindingsLayouts.data();
		}

		if (vkCreateDescriptorSetLayout(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &setLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create a description set layout.");

		_descriptorSetLayout = &descriptorSetLayout;
	}



	void DescriptorSet::initialize() {
		WDE_PROFILE_FUNCTION();
		// Create bindings
		for (auto& binding : _bindingsData) {
			// Uniform buffers or storage buffer
			if (binding._descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || binding._descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
				addBuffer((int) binding._bindingIndex, binding._elementSize, binding._buffer, binding._descriptorType);

				// Image sampler
			else if (binding._descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				addImageSampler((int) binding._bindingIndex, binding._descriptorType, binding._imageView, binding._imageSampler);

				// Storage image
			else if (binding._descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
				addImageStorage((int) binding._bindingIndex, binding._descriptorType, binding._imageView, binding._imageSampler);

				// Input attachment
			else if (binding._descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
				addInputAttachment((int) binding._bindingIndex, binding._renderPassIndex, binding._attachmentBindingIndex);

				// Not implemented
			else
				throw WdeException(LogChannel::RENDER, "Trying to create a descriptor binding type that has not currently been implemented.");
		}
	}

	void DescriptorSet::recreate() {
		WDE_PROFILE_FUNCTION();
		if (_bindingsData.empty())
			return;

		// Update attachments when recreating swap chain
		for (auto& binding : _bindingsData) {
			// Input attachment
			if (binding._descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
				addInputAttachment((int) binding._bindingIndex, binding._renderPassIndex, binding._attachmentBindingIndex);
		}
	}

	void DescriptorSet::updateBuffer(int bindingIndex, const void* newData) {
		if (_bindingsBuffers[bindingIndex] == nullptr || _bindingsBuffers[bindingIndex]->getMemory() == nullptr) // If no buffer memory (ex camera object), return
			return;
		WDE_PROFILE_FUNCTION();

		void* data;
		vkMapMemory(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _bindingsBuffers[bindingIndex]->getMemory(),
		            0, _bindingsBuffers[bindingIndex]->getSize(), 0, &data);
		memcpy(data, newData, _bindingsBuffers[bindingIndex]->getSize());
		vkUnmapMemory(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _bindingsBuffers[bindingIndex]->getMemory());
	}




	// Core functions
	void DescriptorSet::addBuffer(int bindingIndex, int bufferSize, std::shared_ptr<Buffer>& buffer, VkDescriptorType descriptorType) {
		WDE_PROFILE_FUNCTION();

		// Create buffer if necessary
		if (buffer == nullptr) {
			if (descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) { // Storage buffer
				_bindingsBuffers[bindingIndex] = std::make_shared<Buffer>(
						bufferSize,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}
			else { // Simple buffer
				_bindingsBuffers[bindingIndex] = std::make_shared<Buffer>(
						bufferSize,
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}
		}
		else
			_bindingsBuffers[bindingIndex] = buffer;

		// Vulkan binding buffer
		VkDescriptorBufferInfo bufferInfo {};
		bufferInfo.buffer = _bindingsBuffers[bindingIndex]->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = _bindingsBuffers[bindingIndex]->getSize();

		// Write buffers into the set
		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;
		setWrite.dstBinding = bindingIndex;
		setWrite.dstArrayElement = 0; // if we write array, starting array index (default : 0)
		setWrite.dstSet = _descriptorSet;
		setWrite.descriptorCount = 1; // Update 1 descriptor at a time
		setWrite.descriptorType = descriptorType;
		setWrite.pBufferInfo = &bufferInfo;
		setWrite.pImageInfo = nullptr;
		setWrite.pTexelBufferView = nullptr;

		// Update sets
		vkUpdateDescriptorSets(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), 1, &setWrite, 0, nullptr);
	}

	void DescriptorSet::addImageSampler(int bindingIndex, VkDescriptorType descriptorType, VkImageView& imageView, VkSampler& imageSampler) {
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
		setWrite.descriptorType = descriptorType;
		setWrite.pBufferInfo = nullptr;
		setWrite.pImageInfo = &imageInfo;
		setWrite.pTexelBufferView = nullptr;

		// Update sets
		vkUpdateDescriptorSets(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), 1, &setWrite, 0, nullptr);
	}


	void DescriptorSet::addImageStorage(int bindingIndex, VkDescriptorType descriptorType, VkImageView& imageView, VkSampler& imageSampler) {
		WDE_PROFILE_FUNCTION();
		// Create image info
		VkDescriptorImageInfo imageInfo {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
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
		setWrite.descriptorType = descriptorType;
		setWrite.pBufferInfo = nullptr;
		setWrite.pImageInfo = &imageInfo;
		setWrite.pTexelBufferView = nullptr;

		// Update sets
		vkUpdateDescriptorSets(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), 1, &setWrite, 0, nullptr);
	}


	void DescriptorSet::addInputAttachment(int bindingIndex, uint32_t renderPassIndex, uint32_t attachmentBindingIndex) {
		WDE_PROFILE_FUNCTION();

		// Get corresponding image view
		RenderPass& renderPass = WaterDropEngine::get().getInstance().getPipeline().getRenderPass(renderPassIndex);
		auto attachmentType = renderPass.getAttachment(attachmentBindingIndex).type;
		VkImageView* imageView = nullptr;

		switch(attachmentType) {
			case RenderAttachment::Type::SWAPCHAIN: {
				// Not allowed to read from swapchain
				break;
			}
			case RenderAttachment::Type::DEPTH: {
				imageView = &renderPass.getDepthStencil().getView();
				ImageDepth& depthImageTmp = renderPass.getDepthStencil();
				break;
			}
			case RenderAttachment::Type::IMAGE: {
				imageView = &renderPass.getFrameBuffers().getImageAttachment((int) attachmentBindingIndex).getView();
				break;
			}
		}

		// Test if image view exists
		if (imageView == nullptr)
			throw WdeException(LogChannel::RENDER, "Trying to create an image descriptor of an attachment that has a nullptr image view.");

		// Create image info
		VkDescriptorImageInfo imageInfo {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *imageView;
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
		vkUpdateDescriptorSets(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), 1, &setWrite, 0, nullptr);
	}
}
