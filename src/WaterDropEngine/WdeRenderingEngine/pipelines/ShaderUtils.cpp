#include "ShaderUtils.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
	VkShaderModule ShaderUtils::createShaderModule(std::vector<char> shaderCode) {
		// Create infos
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		// Create Module
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(CoreInstance::get().getSelectedDevice().getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw WdeException("Failed to create shader module.", LoggerChannel::RENDERING_ENGINE);

		return shaderModule;
	}



	const std::unordered_map<std::string, VkShaderStageFlagBits> _shaderStagesExtensions = {
			{"comp", VK_SHADER_STAGE_COMPUTE_BIT},
			{"vert", VK_SHADER_STAGE_VERTEX_BIT},
			{"tesc", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
			{"tese", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
			{"geom", VK_SHADER_STAGE_GEOMETRY_BIT},
			{"frag", VK_SHADER_STAGE_FRAGMENT_BIT},
	};

	VkShaderStageFlagBits ShaderUtils::getShaderStage(const std::string &shaderFileName) {
		std::string delimiter = ".";
		std::string str = shaderFileName;

		size_t pos = 0;
		std::string token;

		// Splits file name
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			if (_shaderStagesExtensions.contains(token))
				return _shaderStagesExtensions.at(token);
			str.erase(0, pos + delimiter.length());
		}
		if (_shaderStagesExtensions.contains(str))
			return _shaderStagesExtensions.at(str);

		// If extension not found
		return VK_SHADER_STAGE_ALL;
	}
}
