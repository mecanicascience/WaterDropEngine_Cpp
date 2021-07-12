#include "CoreInstance.hpp"
#include "CoreWindow.hpp"

namespace wde::renderEngine {
	CoreInstance::~CoreInstance() {
		for (CoreDevice &deviceC : devicesList) {
			deviceC.cleanUp();
		}

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);
	}


	// Core functions
	WdeStatus CoreInstance::initialize(CoreWindow &windowCore) {
		// Create a Vulkan instance
		Logger::debug("Creating Vulkan instance.", LoggerChannel::RENDERING_ENGINE);
		createVulkanInstance();

		// Create the surface (link between the Window and the instance)
		Logger::debug("Creating surface.", LoggerChannel::RENDERING_ENGINE);
		createSurface();

		// Setup the debug messenger (uses debugCallback as the layer callback)
		Logger::debug("Creating DebugMessenger callback.", LoggerChannel::RENDERING_ENGINE);
		setupDebugMessenger();

		// Setup every devices and select one (we choose to only use one physical device)
		Logger::debug("Selecting devices.", LoggerChannel::RENDERING_ENGINE);
		setupDevices(windowCore);

		// Return success
		return WdeStatus::WDE_SUCCESS;
	}




	void CoreInstance::createVulkanInstance() {
		// Check if required debug layers are all available
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw WdeException("Validation layers requested, but not available.", LoggerChannel::RENDERING_ENGINE);
		}

		// Vulkan Application Infos
		VkApplicationInfo appInfo {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = Constants::APPLICATION_NAME.c_str();
		appInfo.applicationVersion = Constants::APPLICATION_VERSION;
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Vulkan Application Info struct
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Tell Vulkan that we use GLFW and others extensions
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		// Validation layers
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) { // enable debug layers
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// Create Vulkan instance
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw WdeException("Failed to create Vulkan instance.", LoggerChannel::RENDERING_ENGINE);
		}

		// Check if every required extension is available (avoid VK_ERROR_EXTENSION_NOT_PRESENT)
		hasRequiredExtensions();
	}

	void CoreInstance::createSurface() {
		if (glfwCreateWindowSurface(instance, window.getWindow(), nullptr, &surface) != VK_SUCCESS) {
			throw WdeException("Failed to create window surface.", LoggerChannel::RENDERING_ENGINE);
		}
	}

	void CoreInstance::setupDebugMessenger() {
		if (!enableValidationLayers)
			return;

		// Setup messenger and callbacks
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		// Tell vulkan to use our custom debug messenger
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw WdeException("Failed to set up debug messenger.", LoggerChannel::RENDERING_ENGINE);
		}
	}

	void CoreInstance::setupDevices(CoreWindow &windowCore) {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		Logger::debug("Found " + std::to_string(deviceCount) + " GPU(s).", LoggerChannel::RENDERING_ENGINE);
		if (deviceCount == 0)
			throw WdeException("Failed to find GPUs with Vulkan support", LoggerChannel::RENDERING_ENGINE);


		// Create devices
		for (int i = 0; i < deviceCount; i++) {
			CoreDevice device { *this, i, window.getWindow() };
			devicesList.push_back(device);
		}

		// Setup devices and select main device (in this code, we choose to only use one physical device)
		for (int i = 0; i < devicesList.size(); i++) {
			Logger::debug("== Initializing device n=" + std::to_string(i + 1) + "/" + std::to_string(devicesList.size()) + " ==", LoggerChannel::RENDERING_ENGINE);
			bool isSuitable = devicesList[i].initialize();

			if (isSuitable)
				selectedDeviceId = i;
			Logger::debug("== End initializing device ==", LoggerChannel::RENDERING_ENGINE);
		}

		// Outputs infos
		if (selectedDeviceId == -1)
			throw WdeException("Failed to find a suitable GPU.", LoggerChannel::RENDERING_ENGINE);

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(devicesList[selectedDeviceId].getPhysicalDevice(), &properties);
		Logger::debug("Selected GPU " + std::string(properties.deviceName) + " as default graphics device.", LoggerChannel::RENDERING_ENGINE);
	}



	// Helpers
	std::vector<const char *> CoreInstance::getRequiredExtensions() {
		// Get glfw extensions
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		// Add debug messenger layer if validation layers enabled
		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void CoreInstance::hasRequiredExtensions() {
		// Enable extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// List available extensions
		Logger::debug("Available extensions :", LoggerChannel::RENDERING_ENGINE);
		std::unordered_set<std::string> available;
		for (const auto& extension : extensions) {
			Logger::debug("\t- " + std::string(extension.extensionName), LoggerChannel::RENDERING_ENGINE);

			available.insert(extension.extensionName);
		}
		Logger::debug("", LoggerChannel::RENDERING_ENGINE);


		// List required extensions
		Logger::debug("Required extensions :", LoggerChannel::RENDERING_ENGINE);
		auto requiredExtensions = getRequiredExtensions();
		for (const auto& requiredExtension : requiredExtensions) {
			Logger::debug("\t- " + std::string(requiredExtension), LoggerChannel::RENDERING_ENGINE);

			if (available.find(requiredExtension) == available.end()) {
				throw WdeException("Missing required GLFW extension.", LoggerChannel::RENDERING_ENGINE);
			}
		}
		Logger::debug("", LoggerChannel::RENDERING_ENGINE);
	}


	bool CoreInstance::checkValidationLayerSupport() {
		// List validations layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Check if every required validation layer is enabled
		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	void CoreInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

		// List which layers calls callback
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT // List which message type calls callback
		                         | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		                         | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional
	}



	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			Logger::debug("Validation layer : " + std::string(pCallbackData->pMessage), LoggerChannel::RENDERING_ENGINE);
		else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			Logger::warn("Validation layer : " + std::string(pCallbackData->pMessage), LoggerChannel::RENDERING_ENGINE);
		else
			Logger::err("Validation layer : " + std::string(pCallbackData->pMessage), LoggerChannel::RENDERING_ENGINE);
		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		// Check if debug message function loaded
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks *pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
				instance,
				"vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}
}
