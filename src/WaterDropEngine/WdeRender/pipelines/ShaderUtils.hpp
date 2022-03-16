#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "../../../wde.hpp"

namespace wde::render {
	class ShaderUtils : public NonCopyable {
		public:
			explicit ShaderUtils() = default;

			// Core functions
			/**
			 * Creates a shader module
			 * @param shaderCode The internal source code of the shader compiled in the SPIR-V format
			 * @return The corresponding shader module
			 */
			static VkShaderModule createShaderModule(const std::vector<char>& shaderCode);


			// Getters and setters
			/** @return the corresponding shader stage flag given the shader compiled file name */
			static VkShaderStageFlagBits getShaderStage(const std::string &shaderFileName);
	};
}

