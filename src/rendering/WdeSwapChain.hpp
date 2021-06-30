#pragma once

#include "../core/WdeInstanceDevice.hpp"

namespace wde {
    class WdeSwapChain {
        public:
            WdeSwapChain(WdeInstanceDevice &device, WdeWindow &window) : device{device}, window{window} {};
            ~WdeSwapChain();

            /** Initialize Vulkan device */
            void initialize();

            // Avoid copy
            WdeSwapChain(const WdeSwapChain &) = delete;
            WdeSwapChain &operator=(const WdeSwapChain &) = delete;

            // Getters
            VkExtent2D getSwapChainExtent() { return swapChainExtent; }
            VkRenderPass getRenderPass() { return renderPass; }


        private:
            /** A reference to the WaterDropEngine device */
            WdeInstanceDevice &device;
            /** A reference to the WaterDropEngine window */
            WdeWindow &window;


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

            /** Setup the Vulkan swap-chain to store frames given by GLFW and transmit them to window */
            void createSwapChain();

            /** Create a basic image view for every image in the swap chain (ex : in 3D, each image
             *  has 1 image view for the left eye, and one for the right eye. */
            void createImageViews();

            /** Tell Vulkan about frame-buffers used to render (how many color and depth buffers,
             * how many samples and how to handle their contents) */
            void createRenderPasses();
    };
}
