#pragma once

#include "WdeDevice.hpp"

#include <string>
#include <vector>

/*namespace wde {
    struct PipelineConfigInfo {};

    class WdePipeline {
        public:
            WdePipeline(
                    WdeDevice &device,
                    const std::string &vertFilePath,
                    const std::string &fragFilePath,
                    const PipelineConfigInfo &configInfo
            );
            ~WdePipeline() = default;

            /** Remove possible copies
            WdePipeline(const WdePipeline&) = delete;
            void operator=(const WdePipeline&) = delete;

            static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

        private:
            static std::vector<char> readFile(const std::string &filepath);

            void createGraphicsPipeline(
                    const std::string &vertFilePath,
                    const std::string &fragFilePath,
                    const PipelineConfigInfo &configInfo
            );

            void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

            WdeDevice &wdeDevice;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}*/
