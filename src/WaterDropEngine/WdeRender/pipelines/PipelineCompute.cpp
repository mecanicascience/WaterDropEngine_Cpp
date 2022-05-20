#include "PipelineCompute.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	PipelineCompute::~PipelineCompute() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Cleaning up compute pipeline." << logger::endl;

		// Destroy pipeline
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();
		vkDestroyPipeline(device, _pipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

		// Destroy command buffer reference
		_commandBuffer = nullptr;
	}


	void PipelineCompute::initialize() {
		WDE_PROFILE_FUNCTION();
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Initializing compute pipeline." << logger::endl;


		// Create shaders
		{
			auto shader = WaterDropEngine::get().getResourceManager().load<resource::Shader>(_shaderStage);
			_shaderDescription = shader->getShaderStageCreateInfo();
			_shaderModule = shader->getShaderModule();
		}


		// Create pipeline layout
		{
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;


			// Push constants
			pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(_pushConstantsValues.size());
			if (!_pushConstantsValues.empty())
				pipelineLayoutCreateInfo.pPushConstantRanges = _pushConstantsValues.data();
			else
				pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


			// Descriptor sets
			if (_descriptorSetLayoutsList.empty()) {
				pipelineLayoutCreateInfo.setLayoutCount = 0;
				pipelineLayoutCreateInfo.pSetLayouts = nullptr;
			}
			else {
				pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(_descriptorSetLayoutsList.size());
				pipelineLayoutCreateInfo.pSetLayouts = _descriptorSetLayoutsList.data();
			}


			// Create layout
			if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create compute pipeline layout.");
		}


		// Create pipeline
		{
			VkComputePipelineCreateInfo pipelineCreateInfo {};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.stage = _shaderDescription;
			pipelineCreateInfo.layout = _pipelineLayout;
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineCreateInfo.basePipelineIndex = -1;
			vkCreateComputePipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &_pipeline);
		}

		// Release shaders
		WaterDropEngine::get().getResourceManager().release(_shaderStage);

		// Set initialized
		_initialized = true;
	}
}
