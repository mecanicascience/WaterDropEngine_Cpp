#pragma once

#include "../../core/WdeInstanceDevice.hpp"
#include "WdePipeline.hpp"
#include "WdeRenderer.hpp"
#include "WdeDefaultGraphicsPipeline.hpp"

#include <vulkan/vulkan.h>

namespace wde {
    class WdeRenderer;
    class WdeWindow;

    class WdeSwapChain {
        public:
            WdeSwapChain(WdeInstanceDevice &device, WdeWindow &window) : device{device}, window{window} {};
            ~WdeSwapChain();

            /** Initialize Vulkan device */
            void initialize();
            /** Recreate the swap chain if swap chain no more compatible (like windows size changed) */
            void recreateSwapChain();
            /** Calls destructors to clean up the swap chain */
            void cleanUpSwapChain();


            // Avoid copy
            WdeSwapChain(const WdeSwapChain &) = delete;
            WdeSwapChain &operator=(const WdeSwapChain &) = delete;

            // Getters
            VkExtent2D getSwapChainExtent() { return swapChainExtent; }
            VkRenderPass getRenderPass() { return renderPass; }
            std::vector<VkFramebuffer> getSwapChainFrameBuffers() { return swapChainFramebuffers; }
            VkSwapchainKHR getSwapChain() { return swapChain; }
            std::vector<VkImage> getSwapChainImages() { return swapChainImages; }

            // Setters
            void setRenderer(WdeRenderer *rendererToSet) { this->renderer = rendererToSet; }
            void setGraphicsPipeline(WdePipeline *pipelineToSet) { this->pipeline = pipelineToSet; }

        private:
            WdeInstanceDevice &device;
            WdeWindow &window;
            WdeRenderer *renderer = nullptr;
            WdePipeline *pipeline = nullptr;


            /** The swap chain (stores images to be rendered to the GPU for Vulkan) as VkImages in the swapChainImages vector */
            VkSwapchainKHR swapChain;
            /** Frames waiting to be rendered */
            std::vector<VkImage> swapChainImages;
            /** Image view of each frame (contains display infos about the GPU) */
            std::vector<VkImageView> swapChainImageViews;

            /** The swap-chain render pass */
            VkRenderPass renderPass;


            // Swap chain selected best config
            VkFormat swapChainImageFormat;
            VkExtent2D swapChainExtent;

            /** Holds our framebuffers */
            std::vector<VkFramebuffer> swapChainFramebuffers;

            /** Setup the Vulkan swap-chain to store frames given by GLFW and transmit them to window */
            void createSwapChain();

            /** Create a basic image view for every image in the swap chain (ex : in 3D, each image
             *  has 1 image view for the left eye, and one for the right eye. */
            void createImageViews();

            /** Tell Vulkan about frame-buffers used to render (how many color and depth buffers,
             * how many samples and how to handle their contents) */
            void createRenderPasses();

            /** Create the frame buffers already referenced by the render passes */
            void createFrameBuffers();
    };
}
