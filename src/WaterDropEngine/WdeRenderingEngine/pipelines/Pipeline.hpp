#pragma once

#include "../../../wde.hpp"
#include "../commands/CommandBuffer.hpp"

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


			// Getters and setters
			virtual const VkPipeline &getPipeline() const = 0;
			virtual const VkPipelineBindPoint &getPipelineBindPoint() const = 0;


		protected:
			// Pipeline parameters
			/** true if the pipeline was initialized */
			bool _initialized = false;

			/** The list of all push constants config */
			std::vector<VkPushConstantRange> _pushConstantsValues;
			/** Corresponding binding indices */
			std::unordered_map<int, int> _pushConstantsBoundingIndices;

			/** The corresponding pipeline layout */
			VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
			/** The last binded command buffer */
			CommandBuffer *_commandBuffer = nullptr;
	};
}
