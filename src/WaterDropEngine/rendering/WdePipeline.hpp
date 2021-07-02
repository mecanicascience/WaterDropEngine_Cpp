#pragma once

#include "WdeSwapChain.hpp"

namespace wde {
    class WdeSwapChain; // Pre-declare class

    class WdePipeline {
        public :
            WdePipeline(WdeInstanceDevice &device, WdeSwapChain &swapChain) : device{device}, swapChain{swapChain} {};
            ~WdePipeline() {
                destroyPipeline();
            };


            // Disable copys
            WdePipeline(const WdePipeline &, WdeSwapChain chain) = delete;
            WdePipeline& operator=(const WdePipeline&) = delete;


            /**
             * Initialize the rendering pipeline
             * @param shaderVertLocation
             * @param shaderFragLocation
             */
            void initialize(std::string &shaderVertLocation, std::string &shaderFragLocation) {
                createGraphicsPipeline(shaderVertLocation, shaderFragLocation);
            }

            void destroyPipeline() {
                if (pipelineDestroyed)
                    return;

                vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
                vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);

                pipelineDestroyed = true;
            }

            // Getters
            VkPipeline getGraphicsPipeline() { return graphicsPipeline; }


            /**
             * Creates the graphic pipeline
             * @param shaderVertLocation The local path of the vertex shader
             * @param shaderFragLocation The local path of the fragment shader
             */
            virtual void createGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation) = 0;



        protected:
            /** A reference to the WdeInstanceDevice instance */
            WdeInstanceDevice &device;
            /** A reference to the WdeSwapChainInstance */
            WdeSwapChain &swapChain;

            /** Pipeline layout to pass uniform values (dynamic state variables given to pipeline config) */
            VkPipelineLayout pipelineLayout{};
            /** The main created graphic pipeline */
            VkPipeline graphicsPipeline{};



            // Helper functions
            /**
             * @param code The id of the image view buffer
             * @return the VkShaderModule from the image view buffer
             */
            VkShaderModule createShaderModule(const std::vector<char>& code) {
                // Create infos
                VkShaderModuleCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                createInfo.codeSize = code.size();
                createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

                // Create Module
                VkShaderModule shaderModule;
                if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create shader module.");
                }

                return shaderModule;
            }

            bool pipelineDestroyed = true;
    };
}
