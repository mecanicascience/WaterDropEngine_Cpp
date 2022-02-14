#include "CoreInstance.hpp"

namespace wde::render {
	CoreInstance::CoreInstance(CoreWindow& window) : _window(window) { }

	void CoreInstance::start() {
		WDE_PROFILE_FUNCTION();

		// ========== CREATE VULKAN INSTANCE ==========
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating Vulkan instance." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			// Check if required debug layers are all available
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Checking validation layer support." << logger::endl;
			#ifdef WDE_ENGINE_MODE_DEBUG
				if (!checkValidationLayerSupport())
					throw WdeException(LogChannel::RENDER, "Validation layers requested, but not available.");
			#endif

			// Vulkan Application Infos
			VkApplicationInfo appInfo {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = Config::APPLICATION_NAME.c_str();
			appInfo.applicationVersion = Config::APPLICATION_VERSION;
			appInfo.pEngineName = "No Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = Config::VULKAN_VERSION;

			// Vulkan Application Info struct
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			// Tell Vulkan that we use GLFW and others extensions
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Getting required extensions." << logger::endl;
			auto extensions = getRequiredExtensions();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			// Validation layers
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
			#ifdef WDE_ENGINE_MODE_DEBUG
				// enable debug layers
				createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
				createInfo.ppEnabledLayerNames = _validationLayers.data();

				populateDebugMessengerCreateInfo(debugCreateInfo);
				createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
			#else
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;
			#endif

			// Create Vulkan instance
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating the Vulkan instance." << logger::endl;
			if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
				throw WdeException(LogChannel::RENDER, "Failed to create Vulkan instance.");
			}

			// Check if every required extension is available (avoid VK_ERROR_EXTENSION_NOT_PRESENT)
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Checking if every extension is available." << logger::endl;
			hasRequiredExtensions();
		}

		// ========== CREATE VULKAN SURFACE ==========
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating Vulkan surface." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			if (glfwCreateWindowSurface(_instance, &_window.getWindow(), nullptr, &_surface) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create window surface.");
		}

		// ========== CREATE DEBUGGER MESSAGE CALLBACKS ==========
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating DebugMessenger callback." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			#ifdef WDE_ENGINE_MODE_DEBUG
				// Setup messenger and callbacks
				VkDebugUtilsMessengerCreateInfoEXT createInfo;
				populateDebugMessengerCreateInfo(createInfo);

				// Tell vulkan to use our custom debug messenger
				if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
					throw WdeException(LogChannel::RENDER, "Failed to set up debug messenger.");
			#endif
		}

		// ============ CREATE DEVICE ============
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Initializing devices." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Found " << deviceCount << " GPU(s)." << logger::endl;
			if (deviceCount == 0)
				throw WdeException(LogChannel::RENDER, "Failed to find GPUs with Vulkan support.");


			// Create device
			bool found = false;
			for (int i = 0; i < deviceCount; i++) {
				auto device = std::make_unique<CoreDevice>(i, _window, _instance, _surface);
				if (device->isSuitable()) {
					_device = std::move(device);
					found = true;
				}
			}
			if (!found)
				throw WdeException(LogChannel::RENDER, "Failed to find a suitable GPU.");

			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(_device->getPhysicalDevice(), &properties);
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Selected GPU " << properties.deviceName << " as default graphics device." << logger::endl;
		}

		// ============ CREATE SWAPCHAIN ============
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating Swapchain." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			_swapchain = std::make_unique<Swapchain>();
			_commandBuffers.resize(_swapchain->getImageCount());

			// Create command buffers
			for (size_t i = 0; i < _currentFramesInFlightCount; i++) {
				// Create command commands
				_commandBuffers[i] = std::make_unique<CommandBuffer>(false);
			}
		}
	}

	void CoreInstance::cleanUp() {
		WDE_PROFILE_FUNCTION();
		// Cleanup commands
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Cleaning up command buffers and command pools." << logger::endl;
		_commandBuffers.clear();
		_commandPools.clear();

		// Cleanup swapchain
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Cleaning up swapchain." << logger::endl;
		_swapchain.reset();

		// Cleanup devices
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Cleaning up device." << logger::endl;
		_device.reset();

		// Destroy debug messenger callback
		if (enableValidationLayers()) {
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Cleaning up validation layers." << logger::endl;
			destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
		}

		// Destroy Vulkan surface and instance
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Destroying Vulkan surface and instance." << logger::endl;
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyInstance(_instance, nullptr);
	}



	void CoreInstance::onWindowResized() {
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Swapchain is outdated, recreating it. ==" << logger::endl;

		// Wait if window minimized
		{
			WDE_PROFILE_FUNCTION();
			int width = 0, height = 0;
			glfwGetFramebufferSize(&_window.getWindow(), &width, &height);
			while (width == 0 || height == 0) {
				glfwGetFramebufferSize(&_window.getWindow(), &width, &height);
				glfwWaitEvents();
			}
		}

		// Wait until devices operations finished
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Waiting for device to be ready" << logger::endl;
		waitForDevicesReady();

		// Clean up command buffers and swapchain
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Recreating old swapchain and command buffers." << logger::endl;
		{
			WDE_PROFILE_FUNCTION();
			_swapchain.reset();
			_swapchain = std::make_unique<Swapchain>();

			_commandBuffers.clear();
			_commandBuffers.resize(_swapchain->getImageCount());

			// Create command buffers
			for (size_t i = 0; i < _currentFramesInFlightCount; i++) {
				// Create command commands
				_commandBuffers[i] = std::make_unique<CommandBuffer>(false);
			}
		}

		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Swapchain recreated ==" << logger::endl;
	}









	// ========= Helper functions ===========
	void CoreInstance::waitForDevicesReady() {
		WDE_PROFILE_FUNCTION();
		vkDeviceWaitIdle(_device->getDevice());
	}


	std::vector<const char *> CoreInstance::getRequiredExtensions() {
		// Get glfw extensions
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		// Add debug messenger layer if validation layers enabled
		#ifdef WDE_ENGINE_MODE_DEBUG
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		return extensions;
	}

	void CoreInstance::hasRequiredExtensions() {
		// Enable extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// List available extensions
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Available extensions :" << logger::endl;
		std::unordered_set<std::string> available;
		for (const auto& extension : extensions) {
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "\t- " + std::string(extension.extensionName) << logger::endl;
			available.insert(extension.extensionName);
		}
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << logger::endl;


		// List required extensions
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Required extensions :" << logger::endl;
		auto requiredExtensions = getRequiredExtensions();
		for (const auto& requiredExtension : requiredExtensions) {
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "\t- " + std::string(requiredExtension) << logger::endl;

			if (available.find(requiredExtension) == available.end())
				throw WdeException(LogChannel::RENDER, "Missing required GLFW extension.");
		}
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << logger::endl;
	}


	bool CoreInstance::checkValidationLayerSupport() {
		// List validations layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Check if every required validation layer is enabled
		for (const char* layerName : _validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
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


	// Layer callbacks
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Validation layer : " + std::string(pCallbackData->pMessage) << logger::endl;
		else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			logger::log(LogLevel::WARN, LogChannel::RENDER) << "Validation layer : " + std::string(pCallbackData->pMessage) << logger::endl;
		else
			logger::log(LogLevel::ERR, LogChannel::RENDER) << "Validation layer : " + std::string(pCallbackData->pMessage) << logger::endl;
		return VK_FALSE;
	}

	VkResult createDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		// Check if debug message function loaded
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void destroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks *pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance,"vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}
}
