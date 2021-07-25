#include "SwapChain.hpp"
#include "../../core/CoreInstance.hpp"

namespace wde::renderEngine {
	void SwapChain::initialize() {
		// Create swap chain (stores to this->swapChain and to this->swapChainImages)
		Logger::debug("Creating swapchain.", LoggerChannel::RENDERING_ENGINE);
		createSwapChain();

		// Create a basic image view for every image in the swap chain
		Logger::debug("Creating swapchain image views.", LoggerChannel::RENDERING_ENGINE);
		createImageViews();

		// Create sync objects
		Logger::debug("Creating swapchain fences.", LoggerChannel::RENDERING_ENGINE);
		_imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE); // by default, = VK_NULL
	}

	void SwapChain::cleanUp() {
		VkDevice &device = CoreInstance::get().getSelectedDevice().getDevice();

		// Cleanup fences
		for (auto &imageInFlight : _imagesInFlight) {
			vkDestroyFence(device, imageInFlight, nullptr);
		}

		// Cleanup image views
		for (auto &imageView : _swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		// Destroy swapchain
		vkDestroySwapchainKHR(device, _swapChain, nullptr);
	}






	void SwapChain::createSwapChain() {
		CoreDevice &device = CoreInstance::get().getSelectedDevice();
		SwapChainSupportDetails swapChainSupport = CoreDeviceHelper::querySwapChainSupport(device.getPhysicalDevice());

		// Choose the best image data from what's available
		VkSurfaceFormatKHR surfaceFormat = CoreDeviceHelper::chooseBestSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = CoreDeviceHelper::chooseBestSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = CoreDeviceHelper::chooseBestSwapExtent(CoreInstance::get().getWindow(), swapChainSupport.capabilities);

		// Log output format
		std::stringstream buffer;
		buffer << "Selected a surface format of VkFormat(" << surfaceFormat.format << ") with a color space of VkColorSpaceKHR(" << surfaceFormat.colorSpace << ").";
		Logger::debug(buffer.str(), LoggerChannel::RENDERING_ENGINE);
		Logger::debug("Selected image ratio of " + std::to_string(extent.width) + "x" + std::to_string(extent.height) + " px.", LoggerChannel::RENDERING_ENGINE);

		// We choose to have as possible, the min image (+ 1 to avoid waiting for next image) in the swap chain
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		if (imageCount < CoreInstance::get().getMaxFramesInFlight())
		    CoreInstance::get().setFramesInFlightCount((int) imageCount);
		Logger::debug("The swap-chain will support a count of " + std::to_string(imageCount) + " images.", LoggerChannel::RENDERING_ENGINE);

		// Create struct
		VkSwapchainCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = CoreInstance::get().getSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT if post processing

		// Set queues and swap chain compatibles
		QueueFamilyIndices indices = CoreDeviceHelper::findQueueFamilies(device.getPhysicalDevice());
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		// Can specify images transformations (currentTransform = no transformation)
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE; // Clipping
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// Create swap chain
		if (vkCreateSwapchainKHR(device.getDevice(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
			throw WdeException("Failed to create swap chain.", LoggerChannel::RENDERING_ENGINE);
		}
		Logger::debug("Created swap chain.", LoggerChannel::RENDERING_ENGINE);

		// Say to store swap chain items to swapChainImages
		vkGetSwapchainImagesKHR(device.getDevice(), _swapChain, &imageCount, nullptr);
		_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device.getDevice(), _swapChain, &imageCount, _swapChainImages.data());

		_swapChainImageFormat = surfaceFormat.format;
		_swapChainExtent = extent;
	}

	void SwapChain::createImageViews() {
		// Setup one image view for each image in the swapChain
		_swapChainImageViews.resize(_swapChainImages.size());

		for (size_t i = 0; i < _swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _swapChainImages[i];

			// Select 2D textures
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _swapChainImageFormat;

			// Default color mapping
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// Images used without mipmapping levels or multiple layers
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			// Create image view
			if (vkCreateImageView(CoreInstance::get().getSelectedDevice().getDevice(), &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
				throw WdeException("Failed to create image views.", LoggerChannel::RENDERING_ENGINE);
			}
		}
	}


	// Helper
	VkResult SwapChain::aquireNextImage(VkFence &inFlightFence, VkSemaphore &imageAvailableSemaphore) {
		// Wait for the current frame being rendered at the current frame index
		vkWaitForFences(CoreInstance::get().getSelectedDevice().getDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

		VkResult acquireResult = vkAcquireNextImageKHR(
				CoreInstance::get().getSelectedDevice().getDevice(),
				_swapChain,
				UINT64_MAX, // don't set timeout before image becomes available
				imageAvailableSemaphore, // signaled when presentation engine finished using image
				VK_NULL_HANDLE,
				&_activeImageIndex); // output index of swap chain image that became available (frame is at swapChainImages[imageIndex])

		if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
			throw WdeException("Failed to acquire swap chain image.", LoggerChannel::RENDERING_ENGINE);

		return acquireResult;
	}

	VkResult SwapChain::presentToQueue(VkQueue presentQueue, VkSemaphore& waitSemaphore) {
		VkPresentInfoKHR presentInfo = {};

		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapChain;
		presentInfo.pImageIndices = &_activeImageIndex;

		return vkQueuePresentKHR(presentQueue, &presentInfo);
	}
}
