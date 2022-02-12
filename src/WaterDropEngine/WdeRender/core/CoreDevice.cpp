#include "../../WaterDropEngine.hpp"
#include "CoreDevice.hpp"


namespace wde::render {
	// CORE FUNCTIONS
	CoreDevice::CoreDevice(int deviceID, CoreWindow &window, VkInstance &instance, VkSurfaceKHR &surface) : _window(window), _instance(instance), _surface(surface) {
		// Setup physical device, device, graphics queue, presentation queue
		{
			WDE_PROFILE_FUNCTION();

			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Binding physical device." << logger::endl;
			// Count available physical devices
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

			// List available physical devices
			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

			// Select corresponding physical device
			int i = 0;
			for (const auto& deviceCorr : devices) {
				if (i == deviceID) {
					_physicalDevice = deviceCorr;
					break;
				}
				i++;
			}
		}

		// Retrieve logical graphical and present queues
		{
			WDE_PROFILE_FUNCTION();
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Binding logical device and queues." << logger::endl;
			// Get allowed devices queues types
			QueueFamilyIndices indices = findQueueFamilies();

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
			deviceFeatures.fillModeNonSolid = VK_TRUE;

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();

			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

			// Say to device we use validation layers
			CoreInstance& render = WaterDropEngine::get().getRender().getInstance();
			if (wde::render::CoreInstance::enableValidationLayers()) {
				createInfo.enabledLayerCount = static_cast<uint32_t>(render.getValidationLayers().size());
				createInfo.ppEnabledLayerNames = render.getValidationLayers().data();
			}
			else
				createInfo.enabledLayerCount = 0;

			// Create logical device
			if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create logical device.");

			// Select queues IDs
			uint32_t graphicsQueueIndex = 0;
			uint32_t presentQueueIndex = 0;

			// Add queues handle to the device
			vkGetDeviceQueue(_device, indices.graphicsFamily, graphicsQueueIndex, &_graphicsQueue);
			vkGetDeviceQueue(_device, indices.presentFamily, presentQueueIndex, &_presentQueue);
		}
	}

	CoreDevice::~CoreDevice() {
		// Destroy device
		vkDestroyDevice(_device, nullptr);
	}





	// DEVICE HELPER FUNCTIONS
	bool CoreDevice::isSuitable() {
		QueueFamilyIndices indices = findQueueFamilies();
		bool isCompleteIndicesQueues = indices.isComplete(); // Required queues supported
		bool extensionsSupported = checkDeviceExtensionSupport(); // Required extensions supported

		bool swapChainAdequate = false;
		if (extensionsSupported) { // Supports at least one image renderer type
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport();
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(_physicalDevice, &supportedFeatures);
		bool supportAnisotropy = supportedFeatures.samplerAnisotropy; // Supports sampler Anisotropy

		return isCompleteIndicesQueues && extensionsSupported && swapChainAdequate && supportAnisotropy;
	}



	QueueFamilyIndices CoreDevice::findQueueFamilies() {
		QueueFamilyIndices indices;

		// Count queue family indices
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

		// Get queue family indices
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Find queues that supports VK_QUEUE_GRAPHICS_BLIT && vkGetPhysicalDeviceSurfaceSupportKHR
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, _surface, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
				indices.presentFamilyHasValue = true;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	bool CoreDevice::checkDeviceExtensionSupport() {
		// List every extensions available on the device
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, availableExtensions.data());

		// Check if every required extension is found on the provided device
		std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails CoreDevice::querySwapChainSupport() {
		SwapChainSupportDetails details;

		// Get basic surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &details.capabilities);

		// Query supported formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, details.formats.data());
		}

		// Query supported presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}




	// Config selection
	VkSurfaceFormatKHR CoreDevice::chooseBestSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		// We choose to use VK_FORMAT_B8G8R8A8_SRGB as a SRGB color space if available
		for (const auto& availableFormat : availableFormats)
			if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;

		return availableFormats[0]; // default
	}

	VkPresentModeKHR CoreDevice::chooseBestSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		// We choose to use VK_PRESENT_MODE_MAILBOX_KHR (if buffer empty, use it for parallel computing) if available
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Chose MailBox renderer mode." << logger::endl;
				return availablePresentMode;
			}
		}

		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Chose V-Sync renderer mode." << logger::endl;
		return VK_PRESENT_MODE_FIFO_KHR; // default
	}

	VkExtent2D CoreDevice::chooseBestSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			// Targeted resolution is available
			return capabilities.currentExtent;
		}
		else {
			// Clamp the frame resolution to the image width and height
			int widthLoc, heightLoc;
			glfwGetFramebufferSize(window, &widthLoc, &heightLoc);

			VkExtent2D actualExtent = {
					static_cast<uint32_t>(widthLoc),
					static_cast<uint32_t>(heightLoc)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}
