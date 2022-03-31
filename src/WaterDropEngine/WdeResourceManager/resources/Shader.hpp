#pragma once

#include <utility>
#include "../Resource.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"

namespace wde::resource {
	class Shader : public Resource {
		public:
			explicit Shader(const std::string& path);
			~Shader();
			void drawGUI() override;


			// Shader description
			VkPipelineShaderStageCreateInfo getShaderStageCreateInfo() {
				VkPipelineShaderStageCreateInfo shaderStageInfo {};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = _shaderStageType;
				shaderStageInfo.module = _shaderModule;
				shaderStageInfo.pName = "main"; // entrypoint
				shaderStageInfo.flags = 0; // Optional
				shaderStageInfo.pNext = nullptr; // Optional
				shaderStageInfo.pSpecializationInfo = nullptr; // Optional

				return shaderStageInfo;
			}

			VkShaderModule& getShaderModule() {
				return _shaderModule;
			}


		private:
			VkShaderModule _shaderModule {};
			VkShaderStageFlagBits _shaderStageType {};
	};
}

