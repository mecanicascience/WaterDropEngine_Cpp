#include "SwapChain.hpp"
#include "../core/CoreDevice.hpp"

namespace wde::renderEngine {
	void SwapChain::initialize(GLFWwindow *window, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, VkDevice &device) {
		// Create swap chain (stores to this->swapChain and to this->swapChainImages)
		Logger::debug("Creating Swap-Chain.", LoggerChannel::RENDERING_ENGINE);
		createSwapChain(window, physicalDevice, surface, device);

		// Create a basic image view for every image in the swap chain
		Logger::debug("Creating image views.", LoggerChannel::RENDERING_ENGINE);
		createImageViews(device);
	}

	void SwapChain::cleanUpFrameBuffers(VkDevice &device) {
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		for (auto & depthImageView : depthImageViews) {
			vkDestroyImageView(device, depthImageView, nullptr);
		}
		for (auto & depthImage : depthImages) {
			vkDestroyImage(device, depthImage, nullptr);
		}
		for (auto & depthImageMemory : depthImageMemorys) {
			vkFreeMemory(device, depthImageMemory, nullptr);
		}
	}

	void SwapChain::cleanUpImageViewsAndSwapChain(VkDevice &device) {
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}



	void SwapChain::recreateSwapChain(GLFWwindow *window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface) {
		// Create new swapChain
		createSwapChain(window, physicalDevice, surface, device);

		// Create new image views
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
			std::array<VkImageView, 2> attachments = {swapChainImageViews[i], depthImageViews[i]};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass; // Render pass compatible with the frame buffer
			framebufferInfo.attachmentCount = attachments.size();
			framebufferInfo.pAttachments = attachments.data(); // VkImageVie objects bound to
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1; // Our swap chain are single images

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw WdeException("Failed to create framebuffer.", LoggerChannel::RENDERING_ENGINE);
			}
		}
	}

	void SwapChain::createDepthResources(CoreDevice &device) {
		// Find depth best format
		VkFormat supportedDepthFormat = device.findDepthFormat();

		// Resize values
		depthImages.resize(swapChainImages.size());
		depthImageMemorys.resize(swapChainImages.size());
		depthImageViews.resize(swapChainImages.size());

		for (int i = 0; i < depthImages.size(); i++) {
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = supportedDepthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthImageMemorys[i]);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = depthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = supportedDepthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device.getDevice(), &viewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS) {
				throw WdeException("Failed to create texture image view.", LoggerChannel::RENDERING_ENGINE);
			}
		}
	}
}
