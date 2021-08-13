#include "Descriptor.hpp"
#include "../WdeRenderEngine.hpp"

namespace wde::renderEngine {
	Descriptor::Descriptor(int maxUniformBufferCount, int maxUniformBufferDynamicCount, int maxStorageBufferCount, int maxCombinedImageSamplerCount, int maxInputAttachmentCount) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Creating a new descriptor.", LoggerChannel::RENDERING_ENGINE);
		std::vector<VkDescriptorPoolSize> poolSizes = {
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(maxUniformBufferCount)},
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, static_cast<uint32_t>(maxUniformBufferDynamicCount) },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, static_cast<uint32_t>(maxStorageBufferCount) },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(maxCombinedImageSamplerCount) },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, static_cast<uint32_t>(maxInputAttachmentCount) }
		};
		_pool = std::make_unique<DescriptorPool>(poolSizes);

		// Reference this descriptor to the render instance
	}

	Descriptor::~Descriptor() {
		Logger::debug("Cleaning up descriptor.", LoggerChannel::RENDERING_ENGINE);
		for (int i = 0; i < _layouts.size(); i++)
			vkDestroyDescriptorSetLayout(WdeRenderEngine::get().getSelectedDevice().getDevice(), _layouts[i], nullptr);
	}


	// Core functions
	void Descriptor::initialize() {
		WDE_PROFILE_FUNCTION();

		// Create descriptor sets
		for (auto& [bindingIndex, descriptor] : _descriptorSets) {
			// Create descriptor sets
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.pNext = nullptr;
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = _pool->getPool();
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &_layouts[bindingIndex];

			if (vkAllocateDescriptorSets(WdeRenderEngine::get().getSelectedDevice().getDevice(), &allocInfo, &descriptor->getSet()) != VK_SUCCESS)
				throw WdeException("Failed to allocate descriptor sets.", LoggerChannel::SCENE);

			// Initialize descriptor
			descriptor->initialize();
		}
	}

	void Descriptor::bind(CommandBuffer& commandBuffer, const VkPipelineLayout& layout, VkPipelineBindPoint bindPoint) {
		WDE_PROFILE_FUNCTION();

		// Update every descriptor sets
		for (int i = 0; i < _descriptorSets.size(); i++) { // Must be set in binding index incrementing order
			// Binds the descriptors to the pipeline
			vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, i,
									1, &getSet(i).getSet(), 0, nullptr);
		}
	}

	void Descriptor::createLayouts() {
		WDE_PROFILE_FUNCTION();
		for (int i = 0; i < _descriptorSets.size(); i++) {
			_layouts.push_back(VkDescriptorSetLayout {});
			_descriptorSets[i]->createLayout(_layouts[i]);
		}
	}



	// Getters and setters
	/**
	 * Add a new set to the list
	 * @param bindingsData
	 */
	void Descriptor::addSet(int bindingIndex, const std::vector<DescriptorSetBindingData>& bindingsData) {
		if (bindingsData.empty())
			throw WdeException("Cannot create an empty descriptor set.", LoggerChannel::RENDERING_ENGINE);

		_descriptorSets.emplace(bindingIndex, std::make_unique<DescriptorSet>(bindingsData));
	}

	DescriptorSet& Descriptor::getSet(int bindingIndex) {
		return *_descriptorSets.at(bindingIndex);
	}
}



