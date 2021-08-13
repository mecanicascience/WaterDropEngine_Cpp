#pragma once

#include <utility>

#include "../../../wde.hpp"

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

		// Attachment binding
		uint32_t _attachmentBindingIndex = -1;
		uint32_t _renderPassIndex = -1;

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

		/**
		 * Describes a binding data for an attachment input
		 * @param bindingIndex The index in the descriptor of the binding
		 * @param descriptorType Type of the descriptor binding
		 * @param attachmentBindingIndex The index of the attachment that should be referenced
		 * @param shaderStageFlags In which shaders the descriptor binding will be available (default fragment shaders)
		 */
		DescriptorSetBindingData(uint32_t bindingIndex, VkDescriptorType descriptorType,
								 uint32_t attachmentBindingIndex, uint32_t renderPassIndex, VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT)
			 : _bindingIndex(bindingIndex), _descriptorCount(1), _descriptorType(descriptorType), _attachmentBindingIndex(attachmentBindingIndex),
			   _renderPassIndex(renderPassIndex), _shaderStageFlags(shaderStageFlags) {}
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
			explicit DescriptorSet(std::vector<DescriptorSetBindingData> bindingsData);
			~DescriptorSet();




			// Core functions
			/** Create the new binding layouts given the class data parameters */
			void createBindingLayouts();

			/** Create the descriptor set layout */
			void createLayout(VkDescriptorSetLayout& descriptorSetLayout);



			/** Initialize the descriptor (setup the buffers) */
			void initialize();

			/** Recreate the descriptor set (called when the window is resized) */
			void recreate();

			/**
			 * Update the buffer at the given binding index
			 * @param bindingIndex Buffer index
			 * @param newData The buffer new data
			 */
			void updateBuffer(int bindingIndex, const void* newData);



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
			/** Add a new buffer as a binding */
			void addBuffer(int bindingIndex, int bufferSize);

			/** Add a new image sampler as a binding */
			void addImageSampler(int bindingIndex, VkImageView& imageView, VkSampler& imageSampler);

			/** Add a new input attachment as a binding */
			void addInputAttachment(int bindingIndex, uint32_t renderPassIndex, uint32_t attachmentBindingIndex);

	};
}

