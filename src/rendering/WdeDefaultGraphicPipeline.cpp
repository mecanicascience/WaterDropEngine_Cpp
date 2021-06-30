#include "WdeDefaultGraphicPipeline.hpp"

namespace wde {
    void WdeDefaultGraphicPipeline::initialize(std::string &shaderVertLocation, std::string &shaderFragLocation) {
        createGraphicsPipeline(shaderVertLocation, shaderFragLocation);
    }



    void WdeDefaultGraphicPipeline::createGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation) {
        auto vertShaderCode = readFile(shaderVertLocation);
        auto fragShaderCode = readFile(shaderFragLocation);


    }


    std::vector<char> WdeDefaultGraphicPipeline::readFile(const std::string &fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);

        // Can't open file
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        // Read file
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        // Close and return
        file.close();
        return buffer;
    }
}
