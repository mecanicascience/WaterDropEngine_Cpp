#pragma once

#include "Pipeline.hpp"

namespace wde::render {
	class PipelineCompute : public Pipeline {
		public:
			// Core functions
			explicit PipelineCompute(const std::string& shaderStage) : _shaderStage(shaderStage), _pipelineBindPoint(VK_PIPELINE_BIND_POINT_COMPUTE) {}
			~PipelineCompute() override;

			/** Initialize the pipeline */
			void initialize();


			// Getters and setters
			const VkPipeline &getPipeline() const override { return _pipeline; };
			const VkPipelineBindPoint &getPipelineBindPoint() const override { return _pipelineBindPoint; };
			/**
			 * Add a descriptor set to the pipeline (must be called in binding order, and called before initialize())
			 * @param desc
			 */
			void addDescriptorSet(VkDescriptorSetLayout& desc) { _descriptorSetLayoutsList.push_back(desc); }


		private:
			// Vulkan parameters
			/** Type of the pipeline (compute) */
			VkPipelineBindPoint _pipelineBindPoint;
			/** Descriptor sets list */
			std::vector<VkDescriptorSetLayout> _descriptorSetLayoutsList {};

			// Shader components
			/** The path of the shader */
			std::string _shaderStage;
			/** Shader module description */
			VkPipelineShaderStageCreateInfo _shaderDescription {};
			/** The instance of the shader module */
			VkShaderModule _shaderModule {};
	};
}
