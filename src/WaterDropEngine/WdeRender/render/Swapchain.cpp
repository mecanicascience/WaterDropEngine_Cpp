#include "Swapchain.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	Swapchain::Swapchain() {
		CoreInstance& render = WaterDropEngine::get().getRender().getInstance();

		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating swapchain." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			// Choose the best image data from what's available
			SwapChainSupportDetails swapChainSupport = render.getDevice().querySwapChainSupport();
			VkSurfaceFormatKHR surfaceFormat = CoreDevice::chooseBestSwapSurfaceFormat(swapChainSupport.formats);
			VkPresentModeKHR presentMode = CoreDevice::chooseBestSwapPresentMode(swapChainSupport.presentModes);
			VkExtent2D extent = CoreDevice::chooseBestSwapExtent(WaterDropEngine::get().getRender().getWindow().getWindow(), swapChainSupport.capabilities);

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
			if (vkCreateSwapchainKHR(render.getDevice().getDevice(), &createInfo, nullptr, &_swapchain) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create swap chain.");
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Created swap chain." << logger::endl;

			// Say to store swap chain items to swapChainImages
			vkGetSwapchainImagesKHR(render.getDevice().getDevice(), _swapchain, &imageCount, nullptr);
			_swapChainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(render.getDevice().getDevice(), _swapchain, &imageCount, _swapChainImages.data());

			_swapChainImageFormat = surfaceFormat.format;
			_swapChainExtent = extent;
		}

		// Create an image view for every image in the swapchain
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating swapchain image views." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			// Setup one image view for each image in the swapchain
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
					throw WdeException(LogChannel::RENDER, "Failed to create image view.");
			}
		}

		// Create sync objects
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating swapchain fences." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			_imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE); // By default, = VK_NULL

			// === CREATE THREE SEMAPHORES + 1 CMD BUFFER FOR EACH IMAGE IN THE SWAPCHAIN ===
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating Swapchain sync objects." << logger::endl;
			// Destroy previous fences and semaphores
			for (std::size_t i = 0; i < _inFlightFences.size(); i++) {
				vkDestroyFence(render.getDevice().getDevice(), _inFlightFences[i], nullptr);
				vkDestroySemaphore(render.getDevice().getDevice(), _imageAvailableSemaphores[i], nullptr);
				vkDestroySemaphore(render.getDevice().getDevice(), _renderFinishedSemaphores[i], nullptr);
			}

			// Resize semaphores, fences and command buffers
			int currentFramesInFlightCount = render.getMaxFramesInFlight();
			_imageAvailableSemaphores.resize(currentFramesInFlightCount);
			_renderFinishedSemaphores.resize(currentFramesInFlightCount);
			_inFlightFences.resize(currentFramesInFlightCount);

			// Create structs
			VkSemaphoreCreateInfo semaphoreInfo {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Fence are enabled in init (to launch the program on the first frame renderer)

			for (size_t i = 0; i < currentFramesInFlightCount; i++) {
				// Create semaphores
				if (vkCreateSemaphore(render.getDevice().getDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS
				    || vkCreateSemaphore(render.getDevice().getDevice(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS
				    || vkCreateFence(render.getDevice().getDevice(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
					throw WdeException(LogChannel::RENDER, "Failed to create synchronization objects for the " + std::to_string(i) + "th frame.");
			}
		}

		// Set the rendering engine swapchain current image index to 0
		render.getCurrentFrame() = 0;
	}

	Swapchain::~Swapchain() {
		WDE_PROFILE_FUNCTION();
		VkDevice &device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		// Cleanup sync objects
		for (std::size_t i = 0; i < _inFlightFences.size(); i++) {
			vkDestroyFence(device, _inFlightFences[i], nullptr);
			vkDestroySemaphore(device, _imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(device, _renderFinishedSemaphores[i], nullptr);
		}

		// Cleanup fences
		for (auto &imageInFlight : _imagesInFlight)
			vkDestroyFence(device, imageInFlight, nullptr);

		// Cleanup image views
		for (auto &imageView : _swapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);

		// Destroy swapchain
		vkDestroySwapchainKHR(device, _swapchain, nullptr);
	}



	// Core functions
	VkResult Swapchain::aquireNextImage() {
		CoreInstance &instance = WaterDropEngine::get().getRender().getInstance();
		VkDevice &device = instance.getDevice().getDevice();
		std::size_t currentFrame = instance.getCurrentFrame();

		{
			WDE_PROFILE_FUNCTION();
			// Wait until the last presentation to the queue of this frame is done
			vkWaitForFences(device, 1, &_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		}

		{
			WDE_PROFILE_FUNCTION();
			VkResult acquireResult = vkAcquireNextImageKHR(
					device,
					_swapchain,
					UINT64_MAX, // don't set timeout before image becomes available
					_imageAvailableSemaphores[currentFrame], // signaled when presentation engine finished using image
					VK_NULL_HANDLE,
					&_activeImageIndex); // output index of swap chain image that became available (frame is at swapChainImages[imageIndex])

			if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
				throw WdeException(LogChannel::RENDER, "Failed to acquire swap chain image.");

			return acquireResult;
		}
	}

	VkResult Swapchain::presentToQueue(VkQueue presentQueue) {
		WDE_PROFILE_FUNCTION();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		// Wait for command buffer to be submitted to queue
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[WaterDropEngine::get().getRender().getInstance().getCurrentFrame()];

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapchain;
		presentInfo.pImageIndices = &_activeImageIndex;

		return vkQueuePresentKHR(presentQueue, &presentInfo);
	}
}
