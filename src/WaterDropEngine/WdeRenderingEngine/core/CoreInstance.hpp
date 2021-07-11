#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <unordered_set>
#include <set>

#include "../../WdeCommon/WdeError/WdeStatus.hpp"
#include "../../WdeCommon/WdeError/WdeException.hpp"
#include "../../WdeCommon/WdeUtils/Constants.hpp"
#include "../../WdeCommon/WdeLogger/Logger.hpp"
#include "CoreDevice.hpp"


namespace wde::renderEngine {
	class CoreWindow;

	class CoreInstance {
		public:
			// Instantiation
			/** Create a Vulkan Instance (only 1/app) */
			CoreInstance(CoreWindow &window) : window{window} {};
			/** Destroy the Instance (only 1/app) */
			~CoreInstance();


			// Disables copy
			/** Create a copy of a instance (Not allowed - Only one instance in RenderEngine) */
			CoreInstance(const CoreInstance &) = delete;
			/** Compares two instance (Copy not allowed - Only one instance in RenderEngine) */
			CoreInstance &operator=(const CoreInstance &) = delete;


			// Core functions
			/** Initialize the Vulkan instance */
			WdeStatus initialize(CoreWindow &window);


			// Getters and setters
			VkInstance getVulkanInstance() { return instance; }
			VkSurfaceKHR& getSurface() { return surface; }
			CoreDevice& getSelectedDevice() { return devicesList[selectedDeviceId]; }
			std::vector<CoreDevice>& getDevices() { return devicesList; }
			CoreDevice& getDeviceByID(int id) { return devicesList[id]; }



			// Publics static values
			#ifdef NDEBUG // if compile mode = not debug, disable validation layers
					const bool enableValidationLayers = false;
			#else
					const bool enableValidationLayers = true;
			#endif

			/** Enabled validation layers (VK_LAYER_KHRONOS_validation = every layer enabled) */
			const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};



		private:
			/** The debug messenger callback */
			VkDebugUtilsMessengerEXT debugMessenger;

			/** A reference to the Window */
			CoreWindow &window;

			/** A reference to the Vulkan instance */
			VkInstance instance;
			/** A reference to the surface */
			VkSurfaceKHR surface;

			/** A list of every device in the running machine */
			std::vector<CoreDevice> devicesList;
			/** Main selected device id in array */
			int selectedDeviceId = -1;



			// Core functions
			/** Create a Vulkan instance */
			void createVulkanInstance();

			/** Create the surface (link between the Window and the instance) */
			void createSurface();

			/** Setup the debug messenger Vulkan's layers callback */
			void setupDebugMessenger();

			/** Setup devices list */
			void setupDevices(CoreWindow &windowCore);



			// Helper functions
			/** @return a list of the extensions required by Vulkan */
			std::vector<const char *> getRequiredExtensions();

			/** Check if every required extension are available and loaded (avoid VK_ERROR_EXTENSION_NOT_PRESENT) */
			void hasRequiredExtensions();



			// Debug messages support
			/** @return true if every required validation (= debug) layer are available */
			bool checkValidationLayerSupport();
			/**
			 * Update the debug messenger layers struct
			 * @param createInfo The struct that needs to be updated
			 */
			static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
	};


	/** The debug callback by Vulkan validation layers @return VK_FALSE */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	/** Creates the Vulkan layers Messenger */
	VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	/** Destroy the Vulkan layers Messenger */
	void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks *pAllocator);
}
