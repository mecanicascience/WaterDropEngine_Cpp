#pragma once

#include "../../../wde.hpp"
#include "../core/CoreDevice.hpp"
#include "../core/CoreWindow.hpp"

namespace wde::render {
	/**
	 * Displaying swapchain to the screen
	 */
	class Swapchain {
		public:
			explicit Swapchain();
			~Swapchain();


			// Getters and setters
			int getImageCount() { return (int) _swapChainImageViews.size(); }
			VkFormat getImageFormat() { return _swapChainImageFormat; }
			uint32_t getActiveImageIndex() const { return _activeImageIndex; }
			std::vector<VkImageView> &getImageViews() { return _swapChainImageViews; }
			VkImage& getActiveImage() { return _swapChainImages[_activeImageIndex]; }
			VkExtent2D& getExtent() { return _swapChainExtent; }
			float getAspectRatio() const { return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height); }
			std::vector<VkSemaphore>& getImageAvailableSemaphores() { return _imageAvailableSemaphores; }
			std::vector<VkSemaphore>& getRenderFinishedSemaphores() { return _renderFinishedSemaphores; }
			std::vector<VkFence>& getInFlightFences() { return _inFlightFences; }



			// Core functions
			/**
			 * Bind the current gpu rendering image to the current swapchain image
			 * @return The result of the operation
			 */
			VkResult aquireNextImage();

			/**
			 * Send the current swapchain image to the presentation queue
			 * @param presentQueue
			 * @return The result of the operation
			 */
			VkResult presentToQueue(VkQueue presentQueue);


		private:
			// Vulkan parameters
			/** The swapchain (stores images to be rendered to the GPU for Vulkan) as VkImages in the swapChainImages vector */
			VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
			/** Frames waiting to be rendered */
			std::vector<VkImage> _swapChainImages;
			/** For each image in the swapchain, we bind it with a basic image view */
			std::vector<VkImageView> _swapChainImageViews;
			/** The ID of the current active image index */
			uint32_t _activeImageIndex = UINT32_MAX;

			// Stores swap chain selected configuration
			VkFormat _swapChainImageFormat;
			VkExtent2D _swapChainExtent;

			// Semaphores
			/** Signaled when the corresponding image is used by the GPU (CPU-GPU synchronization) */
			std::vector<VkFence> _inFlightFences;
			/** Signaled when the corresponding image has been acquired and is ready for rendering */
			std::vector<VkSemaphore> _imageAvailableSemaphores;
			/** Signaled when the corresponding image rendering is done, and presentation can happen */
			std::vector<VkSemaphore> _renderFinishedSemaphores;

			// Sync objects
			/** Signals when the corresponding image ends being used by the swapchain */
			std::vector<VkFence> _imagesInFlight;
	};
}
