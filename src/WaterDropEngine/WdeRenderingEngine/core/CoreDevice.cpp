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
		this->graphicsPipeline->setRenderer(renderer);
		this->graphicsPipeline->getRenderer().createRenderPasses(device, swapchain.getImageFormat());

		// Create the graphics pipelines
		this->graphicsPipeline->createGraphicsPipeline(device, swapchain.getSwapChain(), swapchain.getSwapChainExtent(), this->graphicsPipeline->getRenderPass());

		// Create the frame buffers
		swapchain.createFrameBuffers(graphicsPipeline->getRenderPass(), device);

		// Initialize graphics pipelines renderers
		this->graphicsPipeline->getRenderer().initialize(physicalDevice, device, surface, this->graphicsPipeline->getRenderPass(), graphicsPipeline->getPipeline(), swapchain.getSwapChainFrameBuffers(), swapchain.getSwapChainExtent(), swapchain.getSwapChainImages());
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
		graphicsPipeline->getRenderer().createRenderPasses(device, swapchain.getImageFormat());

		// Create new pipelines
		graphicsPipeline->createGraphicsPipeline(device, swapchain.getSwapChain(), swapchain.getSwapChainExtent(), graphicsPipeline->getRenderPass());
		swapchain.createFrameBuffers(graphicsPipeline->getRenderPass(), device);
		graphicsPipeline->getRenderer().createCommandBuffers(device, graphicsPipeline->getPipeline(), swapchain.getSwapChainFrameBuffers(), swapchain.getSwapChainExtent(), graphicsPipeline->getRenderPass());

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

		// Specify used device extensions (allow queues, rendering and optionally validation layers)
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




	bool CoreDevice::isDeviceSuitable(VkPhysicalDevice physicalDeviceRef) {
		QueueFamilyIndices indices = CoreUtils::findQueueFamilies(physicalDeviceRef, instance.getSurface());
		bool isCompleteIndicesQueues = indices.isComplete(); // Required queues supported

		bool extensionsSupported = checkDeviceExtensionSupport(physicalDeviceRef); // Required extensions supported

		bool swapChainAdequate = false;
		if (extensionsSupported) { // Supports at least one image rendering type
			SwapChainSupportDetails swapChainSupport = CoreUtils::querySwapChainSupport(physicalDevice, instance.getSurface());
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(physicalDeviceRef, &supportedFeatures);
		bool supportAnisotropy = supportedFeatures.samplerAnisotropy; // Supports sampler Anisotropy

		return isCompleteIndicesQueues && extensionsSupported && swapChainAdequate && supportAnisotropy;
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
}
