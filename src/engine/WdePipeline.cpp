#include "WdePipeline.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>

/*namespace wde {
    WdePipeline::WdePipeline(
            WdeDevice &device,
            const std::string &vertFilePath,
            const std::string &fragFilePath,
            const PipelineConfigInfo &configInfo)
            : wdeDevice(device) {
        createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
    }

    std::vector<char> WdePipeline::readFile(const std::string &filepath) {
        std::ifstream file{filepath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file : " + filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void WdePipeline::createGraphicsPipeline(
            const std::string &vertFilePath,
            const std::string &fragFilePath,
            const PipelineConfigInfo &configInfo) {
        auto vertCode = this->readFile(vertFilePath);
        auto fragCode = this->readFile(fragFilePath);

        std::cout << "Vertex Shader Code Size : " << vertCode.size() << '\n';
        std::cout << "Fragment Shader Code Size : " << fragCode.size() << '\n';
    }

    void WdePipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(wdeDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
    }

    PipelineConfigInfo WdePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
        PipelineConfigInfo configInfo {};

        return configInfo;
    }
}*/
