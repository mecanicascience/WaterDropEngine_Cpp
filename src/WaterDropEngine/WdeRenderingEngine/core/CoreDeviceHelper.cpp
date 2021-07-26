#include "CoreDeviceHelper.hpp"
#include "CoreInstance.hpp"

namespace wde::renderEngine {
	// Core functions
	void CoreDeviceHelper::setupPhysicalDevice(int deviceID, VkPhysicalDevice &physicalDevice) {
        WDE_PROFILE_FUNCTION();
		// Count available physical devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(CoreInstance::get().getInstance(), &deviceCount, nullptr);

		// List available physical devices
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(CoreInstance::get().getInstance(), &deviceCount, devices.data());

		// Select corresponding physical device
		int i = 0;
		for (const auto& deviceCorr : devices) {
			if (i == deviceID) {
				physicalDevice = deviceCorr;
				break;
			}
			i++;
		}
	}

	void CoreDeviceHelper::setupLogicalDeviceAndQueues(VkPhysicalDevice &physicalDevice, VkDevice &device, VkQueue &graphicsQueue, VkQueue &presentQueue, const std::vector<const char *> &deviceExtensions) {
        WDE_PROFILE_FUNCTION();
	    // Get allowed devices queues types
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		// Specify queues to be created
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Specify used device extensions (allow queues, renderer and optionally validation layers)
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Say to device we use validation layers
		if (CoreInstance::get().enableValidationLayers()) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(CoreInstance::get().getValidationLayers().size());
			createInfo.ppEnabledLayerNames = CoreInstance::get().getValidationLayers().data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		// Create logical device
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw WdeException("Failed to create logical device.", LoggerChannel::RENDERING_ENGINE);
		}

		// Select queues IDs
		uint32_t graphicsQueueIndex = 0;
		uint32_t presentQueueIndex = 0;

		// Add queues handle to the device
		vkGetDeviceQueue(device, indices.graphicsFamily, graphicsQueueIndex, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily, presentQueueIndex, &presentQueue);
	}





	// Helper functions
	QueueFamilyIndices CoreDeviceHelper::findQueueFamilies(VkPhysicalDevice &physicalDevice) {
        WDE_PROFILE_FUNCTION();
	    QueueFamilyIndices indices;

		// Count queue family indices
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		// Get queue family indices
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Find queues that supports VK_QUEUE_GRAPHICS_BLIT && vkGetPhysicalDeviceSurfaceSupportKHR
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, CoreInstance::get().getSurface(), &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
				indices.presentFamilyHasValue = true;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	bool CoreDeviceHelper::isDeviceSuitable(VkPhysicalDevice &physicalDevice, const std::vector<const char *> &deviceExtensions) {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		bool isCompleteIndicesQueues = indices.isComplete(); // Required queues supported

		bool extensionsSupported = checkDeviceExtensionSupport(deviceExtensions); // Required extensions supported

		bool swapChainAdequate = false;
		if (extensionsSupported) { // Supports at least one image renderer type
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);
		bool supportAnisotropy = supportedFeatures.samplerAnisotropy; // Supports sampler Anisotropy

		return isCompleteIndicesQueues && extensionsSupported && swapChainAdequate && supportAnisotropy;
	}



	bool CoreDeviceHelper::checkDeviceExtensionSupport(const std::vector<const char *> &deviceExtensions) {
		VkPhysicalDevice& physicalDevice = CoreInstance::get().getSelectedDevice().getPhysicalDevice();
		// List every extensions available on the device
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

		// Check if every required extension is found on the provided device
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails CoreDeviceHelper::querySwapChainSupport(VkPhysicalDevice &physicalDevice) {
		VkSurfaceKHR &surface = CoreInstance::get().getSurface();
		SwapChainSupportDetails details;

		// Get basic surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

		// Query supported formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
		}

		// Query supported presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}





	// Config selection
	VkSurfaceFormatKHR CoreDeviceHelper::chooseBestSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		// We choose to use VK_FORMAT_B8G8R8A8_SRGB as a SRGB color space if available
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0]; // default
	}

	VkPresentModeKHR CoreDeviceHelper::chooseBestSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		// We choose to use VK_PRESENT_MODE_MAILBOX_KHR (if buffer empty, use it for parallel computing) if available
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				Logger::debug("Chose MailBox renderer mode.", LoggerChannel::RENDERING_ENGINE);
				return availablePresentMode;
			}
		}

		Logger::debug("Chose V-Sync renderer mode.", LoggerChannel::RENDERING_ENGINE);
		return VK_PRESENT_MODE_FIFO_KHR; // default
	}

	VkExtent2D CoreDeviceHelper::chooseBestSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities) {
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
