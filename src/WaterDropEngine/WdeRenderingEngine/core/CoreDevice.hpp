#pragma once

#include <vulkan/vulkan_core.h>
#include <set>

#include "../../WdeCommon/WdeLogger/Logger.hpp"
#include "../../WdeCommon/WdeError/WdeException.hpp"
#include "CoreUtils.hpp"
#include "../renderer/SwapChain.hpp"
#include "../renderer/GraphicsPipeline.hpp"


namespace wde::renderEngine {
	class CoreInstance;
	class CoreDevice {
		public:
			/**
			 * Creates a new device
			 * @param instance A reference of the Vulkan instance
			 * @param deviceId The id of the device in the listDevice() array
			 */
			CoreDevice(CoreInstance &instance, int deviceId, GLFWwindow *window) : instance{instance}, deviceId{deviceId}, window{window} {}
			/** Destruct the device */
			~CoreDevice() = default;
			/** Clean up the device */
			void cleanUp();
			/** Clean up the swapChain full system */
			void cleanUpSwapChain();


			// Core functions
			/** Initialize the device */
			bool initialize();

			/**
			 * Draw frame to the screen
			 * @param window
			 */
			 void drawFrame(CoreWindow &window);


			/**
			 * Recreate the swap chain when needed
			 * @param window A reference to the GLFW Window
			 */
			void recreateSwapChain(GLFWwindow *window);


			// Utils
			/**
			* @param candidates
			* @param tiling
			* @param features
			* @return The perfect candidat between a set of choice
			*/
			VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
			/** Find the compatible depth format */
			VkFormat findDepthFormat();

			/**
			* Create an image based on the provided infos
			* @param imageInfo
			* @param properties
			* @param image
			* @param imageMemory
			*/
			void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);


			// Getters and setters
			VkPhysicalDevice& getPhysicalDevice() { return physicalDevice; }
			VkDevice& getDevice() { return device; };
			VkQueue& getGraphicsQueue() { return graphicsQueue; }
			VkQueue& getPresentQueue() { return presentQueue; }
			int getDeviceID() { return deviceId; }

			void setGraphicsPipeline(VkSurfaceKHR surface, GraphicsPipeline& graphicsPipelineRef, Renderer &renderer);



		private:
			/** Enabled device extensions (VK_KHR_SWAPCHAIN_EXTENSION_NAME = can GPU displays images) */
			const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

			/** Reference to the CoreInstance instance */
			CoreInstance &instance;
			/** Reference to the GLFW window */
			GLFWwindow *window;
			/** Reference to the device swapChain */
			SwapChain swapchain {};
			/** Reference to the binded graphics pipelines */
			GraphicsPipeline* graphicsPipeline {};


			/** Corresponding id of the device (used for initialization only) */
			int deviceId;
			/** True if this device is a suitable Vulkan device */
			bool isThisDeviceSuitable = false;

			/** The corresponding physical device */
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			/** The corresponding logical device */
			VkDevice device;


			// Queues (VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_SPARSE_BINDING_BIT)
			/** Graphics operations queue of the device (contains drawing points, lines, triangles) */
			VkQueue graphicsQueue;
			/** Graphics presentation queue of the device (can present infos to the GPU) */
			VkQueue presentQueue;



			// Core functions
			/** Select the corresponding physical device */
			void selectPhysicalDevice();

			/** Setup the logical device to interact with the physical device */
			void createLogicalDevice();



			// Helper functions
			/**
			 * Check if a physical device supports everything needed
			 * @param physicalDevice
			 * @return true if everything is supported
			 */
			bool isDeviceSuitable(VkPhysicalDevice physicalDevice);

			/**
			 * @param device
			 * @return Return true if the device support the required extensions
			 */
			bool checkDeviceExtensionSupport(VkPhysicalDevice device);

			/**
			 * @param typeFilter
			 * @param properties
			 * @return The type of memory used by the device
			 */
			uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};
}
