#include "CoreDevice.hpp"
#include "CoreInstance.hpp"

namespace wde::renderEngine {
	bool CoreDevice::initialize() {
		// Select the corresponding physical device and check if it supports Vulkan
		Logger::debug("Selecting physical device.", LoggerChannel::RENDERING_ENGINE);
		selectPhysicalDevice();

		// Pick the best logical corresponding device to interact with the physical device (store to this->device)
		Logger::debug("Selecting logical device.", LoggerChannel::RENDERING_ENGINE);
		createLogicalDevice();

		// Create the associated swap chain
		swapchain.initialize(window, getPhysicalDevice(), instance.getSurface(), getDevice());

		return isThisDeviceSuitable;
	}

	// Called by external function
	void CoreDevice::cleanUp() {
		// Clear model
		model->cleanUp(device);
		model = nullptr;

		// Clean up the swapChain full system
		cleanUpSwapChain();

		// Clean up renderers
		graphicsPipeline->getRenderer().cleanUp(device);

		// Destroy device
		vkDestroyDevice(device, nullptr);

		// Delete pointers to window
		window = nullptr;

		// Delete reference to pipelines
		graphicsPipeline->cleanUp();
		graphicsPipeline = nullptr;
	}

	void CoreDevice::cleanUpSwapChain() {
		// Clean Up Frame buffers
		swapchain.cleanUpFrameBuffers(device);

		// Clean up command buffers
		graphicsPipeline->getRenderer().cleanUpCommandBuffers(device);

		// Destroy Pipeline
		graphicsPipeline->cleanUpPipeline(device);

		// Destroy image views and swapChain
		swapchain.cleanUpImageViewsAndSwapChain(device);
	}

	void CoreDevice::setGraphicsPipeline(VkSurfaceKHR surface, GraphicsPipeline& graphicsPipelineRef, Renderer& renderer) {
		// Initialize graphics pipelines
		this->graphicsPipeline = &graphicsPipelineRef;

		// Create swap chain render passes
		Logger::debug("Creating swapchain render passes.", LoggerChannel::RENDERING_ENGINE);
		this->graphicsPipeline->setRenderer(renderer);
		this->graphicsPipeline->getRenderer().createRenderPasses(device, swapchain.getImageFormat(), findDepthFormat());

		// Create the graphics pipelines
		Logger::debug("Creating the graphics pipeline.", LoggerChannel::RENDERING_ENGINE);
		this->graphicsPipeline->createGraphicsPipeline(device, swapchain.getSwapChain(), swapchain.getSwapChainExtent(), this->graphicsPipeline->getRenderPass());

		// Create the command pool (allocate memory to buffers)
		Logger::debug("Creating the command pool.", LoggerChannel::RENDERING_ENGINE);
		this->graphicsPipeline->getRenderer().createCommandPool(physicalDevice, device, surface);

		// Create the depth resources
		Logger::debug("Creating the depth resources.", LoggerChannel::RENDERING_ENGINE);
		swapchain.createDepthResources(*this);

		// Create the frame buffers
		Logger::debug("Creating the frame buffers.", LoggerChannel::RENDERING_ENGINE);
		swapchain.createFrameBuffers(graphicsPipeline->getRenderPass(), device);

		// Initialize graphics pipelines renderers
		Logger::debug("Creating the pipeline renderers.", LoggerChannel::RENDERING_ENGINE);
		this->graphicsPipeline->getRenderer().initialize(*model, physicalDevice, device, surface, this->graphicsPipeline->getRenderPass(), graphicsPipeline->getPipeline(), swapchain.getSwapChainFrameBuffers(), swapchain.getSwapChainExtent(), swapchain.getSwapChainImages());
	}

	void CoreDevice::drawFrame(CoreWindow &window) {
		// Recreate the swapChain if needed (like if a user resized the window)
		if (graphicsPipeline->getRenderer().shouldRecreateSwapChain()) {
			recreateSwapChain(window.getWindow());
			graphicsPipeline->getRenderer().setShouldRecreateSwapChain(false);
			return;
		}

		// Draw the next frame to the window
		graphicsPipeline->drawFrame(window, device, physicalDevice, instance.getSurface(), swapchain, graphicsQueue, presentQueue);
	}

	void CoreDevice::forceDrawFrame(CoreWindow &window) {
		// Draw the next frame to the window
		graphicsPipeline->drawFrame(window, device, physicalDevice, instance.getSurface(), swapchain, graphicsQueue, presentQueue);
	}




	void CoreDevice::recreateSwapChain(GLFWwindow *window) {
		// Handle minimization (wait)
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		// Wait until operations finished
		vkDeviceWaitIdle(device);

		// Destroy last swap chain
		cleanUpSwapChain();

		// Create new swap chain
		swapchain.recreateSwapChain(window, device, physicalDevice, instance.getSurface());
		graphicsPipeline->getRenderer().createRenderPasses(device, swapchain.getImageFormat(), findDepthFormat());

		// Create new pipelines
		graphicsPipeline->createGraphicsPipeline(device, swapchain.getSwapChain(), swapchain.getSwapChainExtent(), graphicsPipeline->getRenderPass());
		swapchain.createDepthResources(*this);
		swapchain.createFrameBuffers(graphicsPipeline->getRenderPass(), device);
		graphicsPipeline->getRenderer().createCommandBuffers(*model, device, graphicsPipeline->getPipeline(), swapchain.getSwapChainFrameBuffers(), swapchain.getSwapChainExtent(), graphicsPipeline->getRenderPass());

		// Resize the imagesInFlight size based on the new swapChainImages size
		graphicsPipeline->getRenderer().getImagesInFlight().resize(swapchain.getSwapChainImages().size(), VK_NULL_HANDLE);
	}





	void CoreDevice::selectPhysicalDevice() {
		// Count available physical devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance.getVulkanInstance(), &deviceCount, nullptr);

		// List available physical devices
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance.getVulkanInstance(), &deviceCount, devices.data());

		// Select corresponding physical device
		int i = 0;
		for (const auto& deviceCorr : devices) {
			if (i == deviceId) {
				physicalDevice = deviceCorr;
				break;
			}
			i++;
		}

		// Say if this device is suitable
		if (isDeviceSuitable(physicalDevice))
			isThisDeviceSuitable = true;
	}

	void CoreDevice::createLogicalDevice() {
		// Get allowed devices queues types
		QueueFamilyIndices indices = CoreUtils::findQueueFamilies(physicalDevice, instance.getSurface());

		// Specify queues to be created
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Specify used device extensions (allow queues, renderer and optionally validation layers)
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Say to device we use validation layers
		if (instance.enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(instance.validationLayers.size());
			createInfo.ppEnabledLayerNames = instance.validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		// Create logical device
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw WdeException("Failed to create logical device.", LoggerChannel::RENDERING_ENGINE);
		}

		// Select queues IDs
		uint32_t graphicsQueueIndex = 0;
		uint32_t presentQueueIndex = 0;

		// Add queues handle to the device
		vkGetDeviceQueue(device, indices.graphicsFamily, graphicsQueueIndex, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily, presentQueueIndex, &presentQueue);
	}




	// Helper functions
	bool CoreDevice::checkDeviceExtensionSupport(VkPhysicalDevice physicalDeviceRef) {
		// List every extensions available on the device
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDeviceRef, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDeviceRef, nullptr, &extensionCount, availableExtensions.data());

		// Check if every required extension is found on the provided device
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool CoreDevice::isDeviceSuitable(VkPhysicalDevice physicalDeviceRef) {
		QueueFamilyIndices indices = CoreUtils::findQueueFamilies(physicalDeviceRef, instance.getSurface());
		bool isCompleteIndicesQueues = indices.isComplete(); // Required queues supported

		bool extensionsSupported = checkDeviceExtensionSupport(physicalDeviceRef); // Required extensions supported

		bool swapChainAdequate = false;
		if (extensionsSupported) { // Supports at least one image renderer type
			SwapChainSupportDetails swapChainSupport = CoreUtils::querySwapChainSupport(physicalDevice, instance.getSurface());
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(physicalDeviceRef, &supportedFeatures);
		bool supportAnisotropy = supportedFeatures.samplerAnisotropy; // Supports sampler Anisotropy

		return isCompleteIndicesQueues && extensionsSupported && swapChainAdequate && supportAnisotropy;
	}

	VkFormat CoreDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			// Get infos about the device and the candidat
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			// Best formats that we want
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw WdeException("Failed to find supported format.", LoggerChannel::RENDERING_ENGINE);
	}

	VkFormat CoreDevice::findDepthFormat() {
		return findSupportedFormat(
				{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}




	void CoreDevice::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) {
		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw WdeException("Failed to create image.", LoggerChannel::RENDERING_ENGINE);
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = RenderingUtils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw WdeException("Failed to allocate image memory.", LoggerChannel::RENDERING_ENGINE);
		}

		if (vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS) {
			throw WdeException("Failed to bind image memory.", LoggerChannel::RENDERING_ENGINE);
		}
	}
}
