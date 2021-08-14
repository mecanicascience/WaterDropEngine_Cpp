#pragma once

#include "../../../wde.hpp"
#include "DescriptorSet.hpp"
#include "DescriptorPool.hpp"
#include "../commands/CommandBuffer.hpp"

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
			explicit Descriptor(int maxUniformBufferCount = 1000, int maxUniformBufferDynamicCount = 1000, int maxStorageBufferCount = 1000, int maxCombinedImageSamplerCount = 1000, int maxInputAttachmentCount = 1000);
			~Descriptor() {
				cleanUp();
			}


			// Core functions
			/** Initialize the descriptor */
			void initialize();
			/** Recreate the descriptor elements (called when the window size changes) */
			void recreate();
			/** Clean up the descriptor */
			void cleanUp();
			/** Bind the descriptor to the pipeline */
			void bind(CommandBuffer& commandBuffer, const VkPipelineLayout& layout, VkPipelineBindPoint bindPoint);

			/** Create the descriptor layouts */
			void createLayouts();
			std::vector<VkDescriptorSetLayout>& getLayouts() { return _layouts; }



			// Getters and setters
			/**
			 * Add a new set to the list
			 * @param bindingsData
			 */
			void addSet(int bindingIndex, const std::vector<DescriptorSetBindingData>& bindingsData);

			DescriptorSet& getSet(int bindingIndex);


		private:
			std::unordered_map<int, std::unique_ptr<DescriptorSet>> _descriptorSets {};
			std::unique_ptr<DescriptorPool> _pool {nullptr};

			// Layouts creation
			std::vector<VkDescriptorSetLayout> _layouts {};
	};
}

