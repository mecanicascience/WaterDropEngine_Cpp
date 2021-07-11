#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace wde::renderEngine {
	/** Struct that handles the device graphics and present queues */
	struct QueueFamilyIndices {
		uint32_t graphicsFamily {};
		uint32_t presentFamily {};
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;

		bool isComplete() {
			return graphicsFamilyHasValue && presentFamilyHasValue;
		}
	};

	/** Struct for the swap-chain (stores images given by external api to a buffer) */
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


	class CoreUtils {
		public:
			/**
			 * Get the swap chain details of the given device
			 * @param physicalDeviceRef
			 * @param &surface
			 * @return The details of the swap chain of the device
			 */
			static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDeviceRef, VkSurfaceKHR &surface);

			/**
			* @param device
			* @param &surface
			* @return The list of queues supported by the device
			*/
			static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR &surface);
	};
}
