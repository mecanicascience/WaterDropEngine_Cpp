#include "CoreUtils.hpp"

namespace wde::renderEngine {
	QueueFamilyIndices CoreUtils::findQueueFamilies(VkPhysicalDevice physicalDeviceRef, VkSurfaceKHR &surface) {
		QueueFamilyIndices indices;

		// Count queue family indices
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceRef, &queueFamilyCount, nullptr);

		// Get queue family indices
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceRef, &queueFamilyCount, queueFamilies.data());

		// Find queues that supports VK_QUEUE_GRAPHICS_BLIT && vkGetPhysicalDeviceSurfaceSupportKHR
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDeviceRef, i, surface, &presentSupport);
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

	SwapChainSupportDetails CoreUtils::querySwapChainSupport(VkPhysicalDevice physicalDeviceRef, VkSurfaceKHR &surface) {
		SwapChainSupportDetails details;

		// Get basic surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceRef, surface, &details.capabilities);

		// Query supported formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceRef, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceRef, surface, &formatCount, details.formats.data());
		}

		// Query supported presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceRef, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceRef, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}
