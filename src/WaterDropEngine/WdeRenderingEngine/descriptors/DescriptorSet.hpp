#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../WdeRenderEngine.hpp"

namespace wde::renderEngine {
	/**
	 * Describes a descriptor set binding
	 */
	struct DescriptorSetBindingData {
		uint32_t _bindingIndex;
		uint32_t _descriptorCount;
		VkDescriptorType _descriptorType;
		VkShaderStageFlags _shaderStageFlags;

		// Buffer
		int _elementSize = 0;

		// Image sampler
		VkImageView _imageView {};
		VkSampler _imageSampler {};

		/**
		 * Describes a binding data for a uniform buffer
		 * @param bindingIndex The index in the descriptor of the binding
		 * @param descriptorCount The number of descriptor in the binding
		 * @param descriptorType Type of the descriptor binding
		 * @param elementSize Size of the element (default 0)
		 * @param shaderStageFlags In which shaders the descriptor binding will be available (default, vertex and fragment shaders)
		 */
		DescriptorSetBindingData(uint32_t bindingIndex, uint32_t descriptorCount, VkDescriptorType descriptorType,
								 int elementSize, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			 : _bindingIndex(bindingIndex), _descriptorCount(descriptorCount), _descriptorType(descriptorType), _elementSize(elementSize), _shaderStageFlags(shaderStageFlags) {}

		/**
		 * Describes a binding data for an image sampler
		 * @param bindingIndex The index in the descriptor of the binding
		 * @param descriptorType Type of the descriptor binding
		 * @param imageView The image view
		 * @param imageSampler The image sampler
		 * @param shaderStageFlags In which shaders the descriptor binding will be available (default, vertex and fragment shaders)
		 */
		DescriptorSetBindingData(uint32_t bindingIndex, VkDescriptorType descriptorType,
							  VkImageView& imageView, VkSampler& imageSampler,
							  VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			  : _bindingIndex(bindingIndex), _descriptorCount(1), _descriptorType(descriptorType), _imageView(imageView), _imageSampler(imageSampler), _shaderStageFlags(shaderStageFlags) {}
	};

	/**
	 * Describes a binding buffer
	 */
	struct DescriptorSetBindingBuffer {
		int bindingIndex;
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		int bufferSize;
	};


	/**
	 * Class that handles a descriptor set
	 */
	class DescriptorSet {
		public:
			// Constructor
			explicit DescriptorSet(std::vector<DescriptorSetBindingData> bindingsData) : _bindingsData(std::move(bindingsData)) {
				WDE_PROFILE_FUNCTION();
				Logger::debug("Creating a descriptor set.", LoggerChannel::RENDERING_ENGINE);

				// Create layouts
				createBindingLayouts();
			}

			~DescriptorSet() {
				// Destroy set buffers
				for (auto& [bindingIndex, bufferStruct] : _bindingsBuffers) {
					vkDestroyBuffer(CoreInstance::get().getSelectedDevice().getDevice(), bufferStruct.buffer, nullptr);
					vkFreeMemory(CoreInstance::get().getSelectedDevice().getDevice(), bufferStruct.bufferMemory, nullptr);
				}

				// Destroy set layout
				_descriptorSetLayout = nullptr;
			}




			// Core functions
			/**
			 * Create the new binding layouts given the class data parameters
			 */
			void createBindingLayouts() {
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

			/**
			 * Create the descriptor set layout
			 */
			void createLayout(VkDescriptorSetLayout& descriptorSetLayout) {
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



			/**
			 * Initialize the descriptor (setup the buffers)
			 */
			void initialize() {
				WDE_PROFILE_FUNCTION();
				// Create bindings
				for (auto& binding : _bindingsData) {
					// Uniform buffers
					if (binding._descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
						addBuffer((int) binding._bindingIndex, binding._elementSize);

					// Image sampler
					else if (binding._descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
						addImageSampler((int) binding._bindingIndex, binding._imageView, binding._imageSampler);

					// Not implemented
					else
						throw WdeException("Trying to create a descriptor binding type that has not currently been implemented.", LoggerChannel::RENDERING_ENGINE);
				}
			}

			/**
			 * Update the buffer at the given binding index
			 * @param bindingIndex Buffer index
			 * @param newData The buffer new data
			 */
			void updateBuffer(int bindingIndex, const void* newData) {
				if (_bindingsBuffers[bindingIndex].bufferMemory == nullptr) // If no buffer memory (ex camera object), return
					return;
				WDE_PROFILE_FUNCTION();

				void* data;
				vkMapMemory(CoreInstance::get().getSelectedDevice().getDevice(), _bindingsBuffers[bindingIndex].bufferMemory,
							0, _bindingsBuffers[bindingIndex].bufferSize, 0, &data);
				memcpy(data, newData, _bindingsBuffers[bindingIndex].bufferSize);
				vkUnmapMemory(CoreInstance::get().getSelectedDevice().getDevice(), _bindingsBuffers[bindingIndex].bufferMemory);
			}



			// Getters and setters
			VkDescriptorSet& getSet() { return _descriptorSet; }



		private:
			// Bindings parameters
			/** The data of the given bindings */
			std::vector<DescriptorSetBindingData> _bindingsData {};
			/** List of binding layouts */
			std::vector<VkDescriptorSetLayoutBinding> _bindingsLayouts {};
			/** The binding buffers based on binding index */
			std::unordered_map<int, DescriptorSetBindingBuffer> _bindingsBuffers {};

			// Vulkan parameters
			VkDescriptorSetLayout* _descriptorSetLayout {};
			VkDescriptorSet _descriptorSet {};


			// Core functions
			void addBuffer(int bindingIndex, int bufferSize) {
				WDE_PROFILE_FUNCTION();
				// Create buffer struct
				_bindingsBuffers[bindingIndex] = DescriptorSetBindingBuffer {};
				_bindingsBuffers[bindingIndex].bindingIndex = bindingIndex;
				_bindingsBuffers[bindingIndex].bufferSize = bufferSize;

				// Create buffer
				BufferUtils::createBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(),
										  CoreInstance::get().getSelectedDevice().getDevice(),
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
				vkUpdateDescriptorSets(CoreInstance::get().getSelectedDevice().getDevice(), 1, &setWrite, 0, nullptr);
			}

			void addImageSampler(int bindingIndex, VkImageView& imageView, VkSampler& imageSampler) {
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
				vkUpdateDescriptorSets(CoreInstance::get().getSelectedDevice().getDevice(), 1, &setWrite, 0, nullptr);
			}

	};
}

