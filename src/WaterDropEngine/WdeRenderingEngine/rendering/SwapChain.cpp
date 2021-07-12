#include "SwapChain.hpp"

namespace wde::renderEngine {
	void SwapChain::cleanUp(VkDevice &device) {
		// Destroy frame buffers
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		// Destroy images views
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		// Destroy swap chain
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	void SwapChain::initialize(GLFWwindow *window, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, VkDevice &device) {
		// Create swap chain (stores to this->swapChain and to this->swapChainImages)
		Logger::debug("Creating Swap-Chain.", LoggerChannel::RENDERING_ENGINE);
		createSwapChain(window, physicalDevice, surface, device);

		// Create a basic image view for every image in the swap chain
		Logger::debug("Creating image views.", LoggerChannel::RENDERING_ENGINE);
		createImageViews(device);
	}



	void SwapChain::recreateSwapChain(GLFWwindow *window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface) {
		// Wait for device
		vkDeviceWaitIdle(device);

		createSwapChain(window, physicalDevice, surface, device);
		createImageViews(device);
	}



	void SwapChain::createSwapChain(GLFWwindow *window, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, VkDevice &device) {
		SwapChainSupportDetails swapChainSupport = CoreUtils::querySwapChainSupport(physicalDevice, surface);

		// Choose the best image data from what's available
		VkSurfaceFormatKHR surfaceFormat = CoreWindow::chooseBestSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = CoreWindow::chooseBestSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = CoreWindow::chooseBestSwapExtent(window, swapChainSupport.capabilities);

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
		Logger::debug("The swap-chain will support a count of " + std::to_string(imageCount) + " images.", LoggerChannel::RENDERING_ENGINE);

		// Create struct
		VkSwapchainCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT if post processing

		// Set queues and swap chain compatibles
		QueueFamilyIndices indices = CoreUtils::findQueueFamilies(physicalDevice, surface);
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
		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw WdeException("Failed to create swap chain.", LoggerChannel::RENDERING_ENGINE);
		}
		Logger::debug("Created swap chain.", LoggerChannel::RENDERING_ENGINE);

		// Say to store swap chain items to swapChainImages
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void SwapChain::createImageViews(VkDevice &device) {
		// Setup one image view for each image in the swapChain
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];

			// Select 2D textures
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;

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
			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
				throw WdeException("Failed to create image views.", LoggerChannel::RENDERING_ENGINE);
			}
		}
	}

	void SwapChain::createFrameBuffers(VkRenderPass &renderPass, VkDevice &device) {
		swapChainFramebuffers.resize(swapChainImageViews.size());

		// Create frame buffers from image views
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			VkImageView attachments[] = { swapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass; // Render pass compatible with the frame buffer
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments; // VkImageVie objects bound to
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1; // Our swap chain are single images

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create framebuffer.");
			}
		}
	}
}
