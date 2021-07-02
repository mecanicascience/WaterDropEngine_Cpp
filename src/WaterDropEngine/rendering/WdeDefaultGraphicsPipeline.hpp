#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include "../core/WdeInstanceDevice.hpp"
#include "WdeSwapChain.hpp"
#include "../utils/Utils.hpp"


namespace wde {
    class WdeDefaultGraphicsPipeline : public WdePipeline {
        public:
            WdeDefaultGraphicsPipeline(WdeInstanceDevice &device, WdeSwapChain &swapChain) : WdePipeline(device, swapChain) {};

        void createGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation) override;
    };
}
