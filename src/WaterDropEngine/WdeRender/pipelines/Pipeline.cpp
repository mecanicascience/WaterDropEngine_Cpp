#include "Pipeline.hpp"

namespace wde::render {
	void Pipeline::bind(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();

		if (!_initialized)
			throw WdeException(LogChannel::RENDER, "Pipeline was bound to command buffer but has not been initialized.");

		_commandBuffer = &commandBuffer;
		vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline());
	}

	void Pipeline::addPushConstants(uint32_t constantsSize, VkShaderStageFlags shaderStages, uint32_t offset) {
		WDE_PROFILE_FUNCTION();
		if (_initialized)
			throw WdeException(LogChannel::RENDER, "Tried to add push constants to pipeline after initialization.");

		// Add push constants config
		VkPushConstantRange pushConstants {};
		pushConstants.size = constantsSize;
		pushConstants.offset = offset;
		pushConstants.stageFlags = shaderStages;
		_pushConstantsValues.push_back(pushConstants);
		_pushConstantsBoundingIndices[0] = (int) _pushConstantsValues.size() - 1;
	}

	void Pipeline::setPushConstants(const void *pushData) {
		WDE_PROFILE_FUNCTION();
		// Push constants to the command buffer
		auto data = _pushConstantsValues[_pushConstantsBoundingIndices.at(0)];
		vkCmdPushConstants(*_commandBuffer, _pipelineLayout, data.stageFlags, data.offset, data.size, pushData);
	}
}
