#pragma once

#include <sstream>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <memory>

#include "../../../../wde.hpp"
#include "../../core/CoreDeviceHelper.hpp"


namespace wde::renderEngine {
	/**
	 * The unique swapchain corresponding to the device and the window, holding frames waiting to be presented to the window
	 */
	class SwapChain : NonCopyable {
		public:
			// Core functions
			/** Initialize swapchain linked to the device */
			void initialize();
			/** Clean up the swapchain */
			void cleanUp();

			// Getters and setters
			int getImageCount() { return (int) _swapChainImageViews.size(); }
			VkFormat getImageFormat() { return _swapChainImageFormat; }
			uint32_t getActiveImageIndex() const { return _activeImageIndex; }
			std::vector<VkImageView> &getImageViews() { return _swapChainImageViews; }
			VkExtent2D& getExtent() { return _swapChainExtent; }

			// Helpers
			/**
			 * Acquire the next image from the swapchain
			 * @param inFlightFence Signals when the corresponding image is done being used by the GPU (CPU-GPU synchronization)
			 * @param imageAvailableSemaphore Signals when the corresponding image has been acquired and is ready for rendering
			 * @return the image acquirement result
			 */
			VkResult aquireNextImage(VkFence &inFlightFence, VkSemaphore &imageAvailableSemaphore);

			/**
			 * Queue an image for presentation using the internal acquired image for queue presentation
			 * @param presentQueue Presentation queue for presenting the image
			 * @param waitSemaphore A optional semaphore that is waited on before the image is presented
			 * @return Result of the queue presentation
			 */
			VkResult presentToQueue(VkQueue presentQueue, VkSemaphore& waitSemaphore);


		private:
			// Vulkan parameters
			/** The swapchain (stores images to be rendered to the GPU for Vulkan) as VkImages in the swapChainImages vector */
			VkSwapchainKHR _swapChain;
			/** Frames waiting to be rendered */
			std::vector<VkImage> _swapChainImages;
			/** For each image in the swapchain, we bind it with a basic image view */
			std::vector<VkImageView> _swapChainImageViews;
			/** The ID of the current active image index */
			uint32_t _activeImageIndex = UINT32_MAX;

			// Stores swap chain selected configuration
			VkFormat _swapChainImageFormat;
			VkExtent2D _swapChainExtent;

			// Sync objects
			/** Signals when the corresponding image ends being used by the swapchain */
			std::vector<VkFence> _imagesInFlight;


			// Core functions
			/** Setup the Vulkan swap-chain to store frames given by GLFW and transmit them to window */
			void createSwapChain();
			/** Create a basic image view for every image in the swap chain (ex : in 3D, each image has 1 image view for the left eye, and one for the right eye. */
			void createImageViews();
	};
}
