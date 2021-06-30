#pragma once

#include "WdeInstanceDevice.hpp"

namespace wde {
    class WdeSwapChain {
        public:
            WdeSwapChain(WdeInstanceDevice &device, WdeWindow &window);
            ~WdeSwapChain();

            /** Initialize Vulkan device */
            void initialize();

            // Avoid copy
            WdeSwapChain(const WdeSwapChain &) = delete;
            WdeSwapChain &operator=(const WdeSwapChain &) = delete;


        private:
            /** A reference to the WaterDropEngine device */
            WdeInstanceDevice &device;
            /** A reference to the WaterDropEngine window */
            WdeWindow &window;


            /** The swap chain (stores images in Vulkan) */
            VkSwapchainKHR swapChain;
            /** VkImages frames list */
            std::vector<VkImage> swapChainImages;
            /** VkImageView for each VkImage */
            std::vector<VkImageView> swapChainImageViews;

            // Swap chain selected best config
            VkFormat swapChainImageFormat;
            VkExtent2D swapChainExtent;

            /** Setup the Vulkan swap-chain to store frames given by GLFW and transmit them to window */
            void createSwapChain();

            /**
             * Create a basic image view for every image in the swap chain (ex : in 3D, each image
             * has 1 image view for the left eye, and one for the right eye.
             */
            void createImageViews();
    };
}
