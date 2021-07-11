#include "CoreWindow.hpp"
#include "../../WdeCommon/WdeLogger/Logger.hpp"

namespace wde::renderEngine {
	CoreWindow::~CoreWindow() {
		glfwDestroyWindow(window);
		glfwTerminate(); // terminates GLFW library
	}


	void CoreWindow::initialize() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not use OpenGL API (since Vulkan used)

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

		// Add callbacks
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void CoreWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		// On new window size
		auto app = reinterpret_cast<CoreWindow*>(glfwGetWindowUserPointer(window));
		app->sendInfoShouldResizeFrameBuffer = true;
	}



	bool CoreWindow::shouldClose() {
		return glfwWindowShouldClose(window);
	}



	VkSurfaceFormatKHR CoreWindow::chooseBestSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		// We choose to use VK_FORMAT_B8G8R8A8_SRGB as a SRGB color space if available
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0]; // default
	}

	VkPresentModeKHR CoreWindow::chooseBestSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		// We choose to use VK_PRESENT_MODE_MAILBOX_KHR if available
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR; // default
	}

	VkExtent2D CoreWindow::chooseBestSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities) {
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
