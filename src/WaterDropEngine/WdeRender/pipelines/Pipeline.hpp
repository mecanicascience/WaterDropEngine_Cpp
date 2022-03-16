#pragma once

#pragma once

#include "../../../wde.hpp"
#include "../commands/CommandBuffer.hpp"

namespace wde::render {
	/**
	 * Class that represents a pipeline
	 */
	class Pipeline : public NonCopyable {
		public:
			// Constructors
			explicit Pipeline() = default;
			~Pipeline() override = default;


			// Core functions
			/**
			 * Binds the specified command buffer to the pipeline
			 * @param commandBuffer
			 */
			void bind(CommandBuffer &commandBuffer);



			// Push constants
			/**
			 * Add push constants to the pipeline
			 * @param constantsSize The size of the push constant data
			 * @param shaderStages The shader stages in which the push constants will be accessible (default : vertex and fragment shaders)
			 * @param offset The offset of the data (default : 0)
			 */
			void addPushConstants(uint32_t constantsSize,
								  VkShaderStageFlags shaderStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
								  uint32_t offset = 0);

			/**
			 * Set the current push constants
			 * @param pushData
			 */
			void setPushConstants(const void* pushData);


			// Getters and setters
			virtual const VkPipeline &getPipeline() const = 0;
			virtual const VkPipelineBindPoint &getPipelineBindPoint() const = 0;
			const VkPipelineLayout &getLayout() const { return _pipelineLayout; }


		protected:
			// Pipeline parameters
			/** true if the pipeline was initialized */
			bool _initialized = false;
			/** The corresponding pipeline */
			VkPipeline _pipeline = VK_NULL_HANDLE;
			/** The corresponding pipeline layout */
			VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
			/** The last binded command buffer */
			CommandBuffer *_commandBuffer = nullptr;

			// Push constants
			/** The list of all push constants config */
			std::vector<VkPushConstantRange> _pushConstantsValues;
			/** Corresponding binding indices */
			std::unordered_map<int, int> _pushConstantsBoundingIndices;
	};
}

