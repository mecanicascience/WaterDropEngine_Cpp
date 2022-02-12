#pragma once

#include <set>

#include "../../../wde.hpp"
#include "CoreWindow.hpp"

namespace wde::render {
	/** Struct that handles the device graphics and present queues */
	struct QueueFamilyIndices {
		uint32_t graphicsFamily {};
		uint32_t presentFamily {};
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;

		bool isComplete() const {
			return graphicsFamilyHasValue && presentFamilyHasValue;
		}
	};

	/** Struct for the swap-chain (stores images given by external api to a buffer) */
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};



	/**
	 * Represents the GPU instance
	 */
	class CoreDevice {
		public:
			/**
			 * Creates the device
			 * @param deviceID ID of the device in the GPU list
			 * @param window
			 * @param instance
			 * @param surface
			 */
			explicit CoreDevice(int deviceID, CoreWindow &window, VkInstance &instance, VkSurfaceKHR &surface);
			~CoreDevice();


			// Getters and setters
			VkPhysicalDevice& getPhysicalDevice() { return _physicalDevice; }
			VkDevice& getDevice() { return _device; }
			VkQueue& getGraphicsQueue() { return _graphicsQueue; }
			VkQueue& getPresentQueue() { return _presentQueue; }


			// Helper functions
			/** @return true if the device is compatible */
			bool isSuitable();
			/** @return the queues supported by the physical device */
			QueueFamilyIndices findQueueFamilies();


			// Helper functions
			/** @return true if the physical device supports required extensions */
			bool checkDeviceExtensionSupport();
			/**
			 * Get the swap chain details of the given device
			 * @return The details of the swap chain of the device
			 */
			SwapChainSupportDetails querySwapChainSupport();



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


		private:
			// Vulkan reference data
			CoreWindow& _window;
			VkInstance& _instance;
			VkSurfaceKHR& _surface;

			// Device vulkan data
			VkPhysicalDevice _physicalDevice = nullptr;
			VkDevice _device = nullptr;

			// Queues (VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_SPARSE_BINDING_BIT)
			VkQueue _graphicsQueue = nullptr;
			VkQueue _presentQueue = nullptr;


			/** Enabled device extensions (VK_KHR_SWAPCHAIN_EXTENSION_NAME = can GPU displays images, VK_KHR_MAINTENANCE1_EXTENSION_NAME = can flip image when presented to viewport) */
			const std::vector<const char *> _deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME };
	};
}
