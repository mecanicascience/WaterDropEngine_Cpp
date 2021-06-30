#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include "../utils/Utils.hpp"
#include "../core/WdeInstanceDevice.hpp"
#include "WdeSwapChain.hpp"

namespace wde {
    class WdeDefaultGraphicsPipeline {
        public:
            WdeDefaultGraphicsPipeline(WdeInstanceDevice &device, WdeSwapChain &swapChain) : device{device}, swapChain{swapChain} {};
            ~WdeDefaultGraphicsPipeline();

            // Disable copys
            WdeDefaultGraphicsPipeline(const WdeDefaultGraphicsPipeline&) = delete;
            WdeDefaultGraphicsPipeline& operator=(const WdeDefaultGraphicsPipeline&) = delete;

            /**
             * Initialize the rendering pipeline
             * @param shaderVertLocation The local path of the vertex shader
             * @param shaderFragLocation The local path of the fragment shader
             */
            void initialize(std::string &shaderVertLocation, std::string &shaderFragLocation);


        private:
            WdeInstanceDevice &device;
            WdeSwapChain &swapChain;

            /** Pipeline layout to pass uniform values (dynamic state variables given to pipeline config) */
            VkPipelineLayout pipelineLayout;
            /** The main created graphic pipeline */
            VkPipeline graphicsPipeline;

            /** Create the graphics pipeline */
            void createGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation);

            /** Create the default graphics pipeline */
            void createDefaultGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation);


            // Helper functions
            /**
             * @param code The id of the image view buffer
             * @return the VkShaderModule from the image view buffer
             */
            VkShaderModule createShaderModule(const std::vector<char>& code);
    };
}
