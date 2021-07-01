#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include "../utils/Utils.hpp"
#include "../core/WdeInstanceDevice.hpp"
#include "WdeSwapChain.hpp"
#include "WdePipeline.hpp"

namespace wde {
    class WdeDefaultGraphicsPipeline : public WdePipeline {
        public:
            WdeDefaultGraphicsPipeline(WdeInstanceDevice &device, WdeSwapChain &swapChain) : WdePipeline(device, swapChain) {};

        void createGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation) override;
    };
}
