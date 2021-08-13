#pragma once

#include "../../../wde.hpp"
#include "DescriptorSet.hpp"
#include "DescriptorPool.hpp"

namespace wde::renderEngine {
	/**
	 * Class that describes multiple descriptor sets
	 */
	class Descriptor {
		public:
			// Constructor
			/**
			 * Create a new descriptor
			 * @param maxUniformBufferCount The maximum number of uniform buffers in the descriptor (default 1000)
			 * @param maxUniformBufferDynamicCount The maximum number of uniform dynamic buffers in the descriptor (default 1000)
			 * @param maxStorageBufferCount The maximum number of storage buffers in the descriptor (default 1000)
			 * @param maxCombinedImageSamplerCount The maximum number of image sampler buffers in the descriptor (default 1000)
			 * @param maxInputAttachmentCount The maximum number of input attachments in the descriptor (default 1000)
			 */
			explicit Descriptor(int maxUniformBufferCount = 1000, int maxUniformBufferDynamicCount = 1000, int maxStorageBufferCount = 1000, int maxCombinedImageSamplerCount = 1000, int maxInputAttachmentCount = 1000) {
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
			}

			~Descriptor() {
				Logger::debug("Cleaning up descriptor.", LoggerChannel::RENDERING_ENGINE);
				for (int i = 0; i < _layouts.size(); i++)
					vkDestroyDescriptorSetLayout(CoreInstance::get().getSelectedDevice().getDevice(), _layouts[i], nullptr);
			}


			// Core functions
			/** Initialize the descriptor */
			void initialize() {
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

					if (vkAllocateDescriptorSets(CoreInstance::get().getSelectedDevice().getDevice(), &allocInfo, &descriptor->getSet()) != VK_SUCCESS)
						throw WdeException("Failed to allocate descriptor sets.", LoggerChannel::SCENE);

					// Initialize descriptor
					descriptor->initialize();
				}
			}

			/** Bind the descriptor to the pipeline */
			void bind(CommandBuffer& commandBuffer, const VkPipelineLayout& layout, VkPipelineBindPoint bindPoint) {
				WDE_PROFILE_FUNCTION();

				// Update every descriptor sets
				for (int i = 0; i < _descriptorSets.size(); i++) { // Must be set in binding index incrementing order
					// Binds the descriptors to the pipeline
					vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, i,
											1, &getSet(i).getSet(), 0, nullptr);
				}
			}

			/** Create the descriptor layouts */
			void createLayouts() {
				WDE_PROFILE_FUNCTION();
				for (int i = 0; i < _descriptorSets.size(); i++) {
					_layouts.push_back(VkDescriptorSetLayout {});
					_descriptorSets[i]->createLayout(_layouts[i]);
				}
			}
			std::vector<VkDescriptorSetLayout>& getLayouts() { return _layouts; }



			// Getters and setters
			/**
			 * Add a new set to the list
			 * @param bindingsData
			 */
			void addSet(int bindingIndex, const std::vector<DescriptorSetBindingData>& bindingsData) {
				if (bindingsData.empty())
					throw WdeException("Cannot create an empty descriptor set.", LoggerChannel::RENDERING_ENGINE);

				_descriptorSets.emplace(bindingIndex, std::make_unique<DescriptorSet>(bindingsData));
			}

			DescriptorSet& getSet(int bindingIndex) {
				return *_descriptorSets.at(bindingIndex);
			}


		private:
			std::unordered_map<int, std::unique_ptr<DescriptorSet>> _descriptorSets {};
			std::unique_ptr<DescriptorPool> _pool {nullptr};

			// Layouts creation
			std::vector<VkDescriptorSetLayout> _layouts {};
	};
}

