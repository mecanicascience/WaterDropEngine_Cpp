#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm> // Necessary for std::min/std::max

namespace wde {
    /**
     * The main WaterDropEngine window
     */
    class WdeWindow {
        public:
            /**
             * Creates a new glfw window
             * @param width
             * @param height
             * @param windowName
             */
            WdeWindow(int width, int height, std::string name);

            /** Destructs the window */
            ~WdeWindow();

            /** Create a copy of a window (Not allowed - Only one window) */
            WdeWindow(const WdeWindow &) = delete;
            /** Compares two windows (Copy not allowed - Only one window) */
            WdeWindow &operator=(const WdeWindow &) = delete;


            /** Initialisation of the window */
            void initialize();

            /** Run the events */
            void run();

            /** @return true if the window should close, false instead */
            bool shouldClose();

            /**
             * Create a GLSW window
             * @param instance The current Vulkan instance
             * @param surface A reference to the surface (link between the window and Vulkan)
             */
            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);



            /**
             * Choose the best swap surface format for rendering (color depth)
             * @param availableFormats
             * @return The best available format (preferably VK_FORMAT_B8G8R8A8_SRGB as a SRGB color space)
             */
            VkSurfaceFormatKHR chooseBestSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

            /**
             * Choose the best presentation mode (frame rendering mode)
             * @param availablePresentModes
             * @return The best available presentation mode (preferably VK_PRESENT_MODE_MAILBOX_KHR)
             */
            VkPresentModeKHR chooseBestSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

            /**
             * Choose the best swap extent mode (frame rendering resolution)
             * @param capabilities
             * @return The best swap extent mode (preferably the same as the pixel width and height of the image)
             */
            VkExtent2D chooseBestSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);



        private:
            const int width;
            const int height;

            std::string windowName;

            GLFWwindow *window;
    };
}