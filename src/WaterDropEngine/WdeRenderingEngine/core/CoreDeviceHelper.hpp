#pragma once

#include <vulkan/vulkan_core.h>
#include <set>
#include <GLFW/glfw3.h>

#include "../../../wde.hpp"


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


	class CoreDeviceHelper {
		public:
			// Core functions
			/** Setup the corresponding physical device */
			static void setupPhysicalDevice(int deviceID, VkPhysicalDevice &physicalDevice);
			/** Setup the logical device to interact with the physical device */
			static void setupLogicalDeviceAndQueues(VkPhysicalDevice &physicalDevice, VkDevice &device, VkQueue &graphicsQueue, VkQueue &presentQueue, const std::vector<const char *> &deviceExtensions);
			/** Setup the rendering engine command pool */
			static void setupCommandPool(VkPhysicalDevice &physicalDevice, VkDevice &device, VkCommandPool &commandPool);


			// Helper functions
			/** @return true if the physical device supports everything needed */
			static bool isDeviceSuitable(VkPhysicalDevice &physicalDevice, const std::vector<const char *> &deviceExtensions);
			/** @return the queues supported by the physical device */
			static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice &physicalDevice);

			/**
			 * @param deviceExtensions
			 * @return true if the physical device supports required extensions
			 */
			static bool checkDeviceExtensionSupport(const std::vector<const char *> &deviceExtensions);
			/**
			 * Get the swap chain details of the given device
			 * @param physicalDevice
			 * @return The details of the swap chain of the device
			 */
			static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice &physicalDevice);



			// Config selection
			/**
			 * Choose the best swap surface format for renderer (color depth)
			 * @param availableFormats
			 * @return The best available format (preferably VK_FORMAT_B8G8R8A8_SRGB as a SRGB color space)
			 */
			static VkSurfaceFormatKHR chooseBestSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

			/**
			 * Choose the best presentation mode (frame renderer mode)
			 * @param availablePresentModes
			 * @return The best available presentation mode (preferably VK_PRESENT_MODE_MAILBOX_KHR)
			 */
			static VkPresentModeKHR chooseBestSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

			/**
			 * Choose the best swap extent mode (frame renderer resolution)
			 * @param *window
			 * @param capabilities
			 * @return The best swap extent mode (preferably the same as the pixel width and height of the image)
			 */
			static VkExtent2D chooseBestSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities);
	};
}
