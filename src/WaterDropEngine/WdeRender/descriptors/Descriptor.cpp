#include "Descriptor.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	Descriptor::Descriptor(int maxUniformBufferCount, int maxUniformBufferDynamicCount, int maxStorageBufferCount, int maxCombinedImageSamplerCount, int maxInputAttachmentCount) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating a new descriptor." << logger::endl;
		std::vector<VkDescriptorPoolSize> poolSizes = {
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(maxUniformBufferCount)},
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, static_cast<uint32_t>(maxUniformBufferDynamicCount) },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, static_cast<uint32_t>(maxStorageBufferCount) },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(maxCombinedImageSamplerCount) },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, static_cast<uint32_t>(maxInputAttachmentCount) }
		};
		_pool = std::make_unique<DescriptorPool>(poolSizes);
	}



	// Core functions
	void Descriptor::initialize() {
		WDE_PROFILE_FUNCTION();

		// Make sure layouts are created
		createLayouts();

		// Create descriptor sets
		for (auto& [bindingIndex, descriptor] : _descriptorSets) {
			// Create descriptor sets
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.pNext = nullptr;
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = _pool->getPool();
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &_layouts[bindingIndex];

			if (vkAllocateDescriptorSets(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &allocInfo, &descriptor->getSet()) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to allocate descriptor sets.");

			// Initialize descriptor
			descriptor->initialize();
		}
	}

	Descriptor::~Descriptor() {
		// Dereference descriptors and pool
		_descriptorSets.clear();
		_pool.reset();

		for (auto& _layout : _layouts)
			vkDestroyDescriptorSetLayout(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _layout, nullptr);
	}

	void Descriptor::recreate() {
		WDE_PROFILE_FUNCTION();

		// Recreate sets
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Recreating a descriptor." << logger::endl;
		for (auto& set : _descriptorSets)
			set.second->recreate();
	}

	void Descriptor::bind(CommandBuffer& commandBuffer, Pipeline& pipeline) {
		WDE_PROFILE_FUNCTION();

		// Update every descriptor sets
		for (int i = 0; i < _descriptorSets.size(); i++) { // Must be set in binding index incrementing order
			// Binds the descriptors to the pipeline
			vkCmdBindDescriptorSets(commandBuffer, pipeline.getPipelineBindPoint(), pipeline.getLayout(), i,
			                        1, &getSet(i).getSet(), 0, nullptr);
		}
	}

	void Descriptor::createLayouts() {
		WDE_PROFILE_FUNCTION();

		// Create descriptor layouts in descriptor order
		for (int descriptorSetID = 0; descriptorSetID < _descriptorSets.size(); descriptorSetID++) {
			// Find descriptor corresponding to this ID
			bool found = false;
			for (auto& descriptor : _descriptorSets) {
				if (descriptor.first == descriptorSetID) {
					// Create descriptor layout
					if (descriptor.second == nullptr)
						throw WdeException(LogChannel::RENDER, "Descriptor set " + std::to_string(descriptorSetID) + " is undefined.");

					VkDescriptorSetLayout layout {};
					_layouts.push_back(layout);
					descriptor.second->createLayout(_layouts[_layouts.size() - 1]);

					// Say that we found the descriptor at the searched ID
					found = true;
					break;
				}
			}

			// Found set N but did not find set N - 1
			if (!found)
				throw WdeException(LogChannel::RENDER, "Failed to create descriptor layouts. Didn't found set " + std::to_string(descriptorSetID)
				                   + " but sets are supposed to go up to " + std::to_string(_descriptorSets.size()) + " sets.");
		}

		_layoutsCreated = true;
	}



	// Getters and setters
	/**
	 * Add a new set to the list
	 * @param bindingsData
	 */
	void Descriptor::addSet(int bindingIndex, const std::vector<DescriptorSetBindingData>& bindingsData) {
		if (bindingsData.empty())
			throw WdeException(LogChannel::RENDER, "Cannot create an empty descriptor set.");

		_descriptorSets.emplace(bindingIndex, std::make_unique<DescriptorSet>(bindingsData));
	}

	DescriptorSet& Descriptor::getSet(int bindingIndex) {
		return *_descriptorSets.at(bindingIndex);
	}
}
