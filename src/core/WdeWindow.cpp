#include "WdeWindow.hpp"

namespace wde {
    WdeWindow::~WdeWindow() {
        glfwDestroyWindow(this->window);
        glfwTerminate(); // terminates GLFW library
    }


    void WdeWindow::initialize() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not use OpenGL API (since Vulkan used)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Currently not resizable (not implemented)

        this->window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void WdeWindow::run() {
        glfwPollEvents();
    }

    bool WdeWindow::shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void WdeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface.");
        }
    }



    VkSurfaceFormatKHR WdeWindow::chooseBestSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        // We choose to use VK_FORMAT_B8G8R8A8_SRGB as a SRGB color space if available
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0]; // default
    }

    VkPresentModeKHR WdeWindow::chooseBestSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        // We choose to use VK_PRESENT_MODE_MAILBOX_KHR if available
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR; // default
    }

    VkExtent2D WdeWindow::chooseBestSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            // Targeted resolution is available
            return capabilities.currentExtent;
        }
        else {
            // Clamp the frame resolution to the image width and height
            int widthLoc, heightLoc;
            glfwGetFramebufferSize(window, &widthLoc, &heightLoc);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(widthLoc),
                static_cast<uint32_t>(heightLoc)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}

