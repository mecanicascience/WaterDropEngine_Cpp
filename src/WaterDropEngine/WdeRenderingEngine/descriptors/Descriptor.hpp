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
			 * @param maxUniformBufferCount The maximum number of uniform buffer in the descriptor (default 10)
			 */
			explicit Descriptor(int maxUniformBufferCount = 10) {
				WDE_PROFILE_FUNCTION();
				Logger::debug("Creating a new descriptor.", LoggerChannel::RENDERING_ENGINE);
				std::vector<VkDescriptorPoolSize> poolSizes = {
					{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(maxUniformBufferCount)}
				};
				_pool = std::make_unique<DescriptorPool>(poolSizes);
			}

			~Descriptor() {
				Logger::debug("Cleaning up descriptor.", LoggerChannel::RENDERING_ENGINE);
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
					allocInfo.pSetLayouts = &descriptor.getLayout();

					if (vkAllocateDescriptorSets(CoreInstance::get().getSelectedDevice().getDevice(), &allocInfo, &descriptor.getSet()) != VK_SUCCESS)
						throw WdeException("Failed to allocate descriptor sets.", LoggerChannel::SCENE);

					// Initialize descriptor
					descriptor.initialize();
				}
			}

			/** Bind the descriptor to the pipeline */
			void bind(CommandBuffer& commandBuffer, const VkPipelineLayout& layout, VkPipelineBindPoint bindPoint) {
				WDE_PROFILE_FUNCTION();
				// Update every descriptor sets
				for (auto& [bindingIndex, set] : _descriptorSets) {
					vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, 0,
											1, &set.getSet(), 0, nullptr);
				}
			}


			// Helper functions
			/** Create the descriptor layouts */
			std::vector<VkDescriptorSetLayout> createLayouts() {
				WDE_PROFILE_FUNCTION();
				std::vector<VkDescriptorSetLayout> layouts {};
				for (auto& [bindingIndex, descriptor] : _descriptorSets) {
					descriptor.createLayout();
					layouts.push_back(descriptor.getLayout());
				}
				return layouts;
			}



			// Getters and setters
			/**
			 * Add a new set to the list
			 * @param bindingsData
			 */
			void addSet(int bindingIndex, const std::vector<DescriptorSetBindingData>& bindingsData) {
				_descriptorSets.emplace(bindingIndex, DescriptorSet {bindingsData});
			}

			DescriptorSet& getSet(int bindingIndex) {
				return _descriptorSets.at(bindingIndex);
			}


		private:
			std::unordered_map<int, DescriptorSet> _descriptorSets {};
			std::unique_ptr<DescriptorPool> _pool {nullptr};
	};
}

