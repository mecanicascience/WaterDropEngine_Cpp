#pragma once

#include "../../../wde.hpp"
#include "../commands/CommandBuffer.hpp"
#include "../descriptors/DescriptorSet.hpp"
#include "../descriptors/Descriptor.hpp"

namespace wde::renderEngine {
	/**
	 * Class that represents a pipeline
	 */
	class Pipeline : NonCopyable {
		public:
			// Constructors
            explicit Pipeline() = default;
			virtual ~Pipeline() = default;

			/** Initialize the pipeline */
			virtual void initialize() = 0;


			// Core functions
			/**
			 * Binds the specified command buffer to the pipeline
			 * @param commandBuffer
			 */
			void bind(CommandBuffer &commandBuffer) {
                WDE_PROFILE_FUNCTION();

                if (!_initialized)
                	throw WdeException("Pipeline was bind and has not been initialized.", LoggerChannel::RENDERING_ENGINE);

				_commandBuffer = &commandBuffer;
				vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline());
			}

			/**
			 * Binds the descriptor to the pipeline and to the command buffer (must be called after the bind command)
			 * @param descriptor
			 */
			void bind(std::shared_ptr<Descriptor>& descriptor) {
				WDE_PROFILE_FUNCTION();
				if (!_commandBuffer)
					throw WdeException("Pipeline descriptor was bind before being bind to the command buffer.", LoggerChannel::RENDERING_ENGINE);

				descriptor->bind(*_commandBuffer, getLayout(), getPipelineBindPoint());
			}



			// Push constants
			/**
			 * Add push constants to the pipeline
			 * @param bindingIndex The corresponding push constants binding index
			 * @param constantsSize The size of the push constant data
			 * @param shaderStages The shader stages in which the push constants will be accessible (default : vertex and fragment shaders)
			 * @param offset The offset of the data (default : 0)
			 */
			void addPushConstants(int bindingIndex, uint32_t constantsSize, VkShaderStageFlags shaderStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, uint32_t offset = 0) {
				if (_initialized)
					throw WdeException("Tried to add push constants to pipeline after initialization.", LoggerChannel::RENDERING_ENGINE);

				// Add push constants config
				VkPushConstantRange pushConstants {};
				pushConstants.size = constantsSize;
				pushConstants.offset = offset;
				pushConstants.stageFlags = shaderStages;
				_pushConstantsValues.push_back(pushConstants);
				_pushConstantsBoundingIndices[bindingIndex] = (int) _pushConstantsValues.size() - 1;
			}

			/**
			 * Set the current push constants
			 * @param bindingIndex
			 * @param pushData
			 */
			void setPushConstants(int bindingIndex, const void* pushData) {
				// Push constants to the command buffer
				auto data = _pushConstantsValues[_pushConstantsBoundingIndices.at(bindingIndex)];
				vkCmdPushConstants(*_commandBuffer, _pipelineLayout, data.stageFlags, data.offset, data.size, pushData);
			}

			/**
			 * Set the pipeline optional descriptor
			 * @param descriptor An instance of a descriptor
			 */
			void setDescriptor(std::shared_ptr<Descriptor> &descriptor) {
				if (_initialized)
					throw WdeException("Tying to set a pipeline descriptor after it's creation.", LoggerChannel::RENDERING_ENGINE);

				_descriptor = descriptor;
			}





			// Getters and setters
			virtual const VkPipeline &getPipeline() const = 0;
			virtual const VkPipelineBindPoint &getPipelineBindPoint() const = 0;
			VkPipelineLayout const &getLayout() const { return _pipelineLayout; }


		protected:
			// Pipeline parameters
			/** true if the pipeline was initialized */
			bool _initialized = false;
			/** The corresponding pipeline layout */
			VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
			/** The last binded command buffer */
			CommandBuffer *_commandBuffer = nullptr;

			// Push constants
			/** The list of all push constants config */
			std::vector<VkPushConstantRange> _pushConstantsValues;
			/** Corresponding binding indices */
			std::unordered_map<int, int> _pushConstantsBoundingIndices;

			// Descriptors
			std::shared_ptr<Descriptor> _descriptor {nullptr};
	};
}
