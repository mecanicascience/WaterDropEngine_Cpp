#include "Swapchain.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	Swapchain::Swapchain() {
		CoreInstance& render = WaterDropEngine::get().getRender().getInstance();

		// Create swapchain (stores to this->swapChain and to this->swapChainImages)
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating swapchain." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			// Choose the best image data from what's available
			SwapChainSupportDetails swapChainSupport = render.getDevice().querySwapChainSupport();
			VkSurfaceFormatKHR surfaceFormat = CoreDevice::chooseBestSwapSurfaceFormat(swapChainSupport.formats);
			VkPresentModeKHR presentMode = CoreDevice::chooseBestSwapPresentMode(swapChainSupport.presentModes);
			VkExtent2D extent = CoreDevice::chooseBestSwapExtent(&WaterDropEngine::get().getRender().getWindow().getWindow(), swapChainSupport.capabilities);

			// Log output format
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Selected a surface format of VkFormat(" << surfaceFormat.format << ") with a color space of VkColorSpaceKHR(" << surfaceFormat.colorSpace << ")." << logger::endl;
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Selected image ratio of " << extent.width << "x" << extent.height << " px." << logger::endl;

			// We choose to have as possible, the min image (+ 1 to avoid waiting for next image) in the swap chain
			uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
			if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
				imageCount = swapChainSupport.capabilities.maxImageCount;
			if (imageCount < render.getMaxFramesInFlight())
				render.setFramesInFlightCount((int) imageCount);
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "The swap-chain will support a count of " << imageCount << " images." << logger::endl;

			// Create struct
			VkSwapchainCreateInfoKHR createInfo {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = render.getSurface();
			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT if post processing

			// Set queues and swap chain compatibles
			QueueFamilyIndices indices = render.getDevice().findQueueFamilies();
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
			if (vkCreateSwapchainKHR(render.getDevice().getDevice(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create swap chain.");
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Created swap chain." << logger::endl;

			// Say to store swap chain items to swapChainImages
			vkGetSwapchainImagesKHR(render.getDevice().getDevice(), _swapChain, &imageCount, nullptr);
			_swapChainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(render.getDevice().getDevice(), _swapChain, &imageCount, _swapChainImages.data());

			_swapChainImageFormat = surfaceFormat.format;
			_swapChainExtent = extent;
		}

		// Create a basic image view for every image in the swap chain
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating swapchain image views." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
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
				if (vkCreateImageView(render.getDevice().getDevice(), &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
					throw WdeException(LogChannel::RENDER, "Failed to create image views.");
			}
		}

		// Create sync objects
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating swapchain fences." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			_imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE); // By default, = VK_NULL
		}
	}

	Swapchain::~Swapchain() {
		WDE_PROFILE_FUNCTION();
		VkDevice &device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		// Cleanup fences
		for (auto &imageInFlight : _imagesInFlight)
			vkDestroyFence(device, imageInFlight, nullptr);

		// Cleanup image views
		for (auto &imageView : _swapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);

		// Destroy swapchain
		vkDestroySwapchainKHR(device, _swapChain, nullptr);
	}
}
