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
			VkExtent2D& getExtent() { return _swapChainExtent; }
			float getAspectRatio() const { return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height); }


		private:
			// Vulkan parameters
			/** The swapchain (stores images to be rendered to the GPU for Vulkan) as VkImages in the swapChainImages vector */
			VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
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
	};
}
