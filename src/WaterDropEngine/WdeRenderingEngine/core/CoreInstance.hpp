#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <unordered_set>
#include <set>
#include <map>

#include "../../../wde.hpp"
#include "CoreDevice.hpp"
#include "../renderer/Renderer.hpp"
#include "../commands/CommandBuffer.hpp"
#include "../commands/CommandPool.hpp"
#include "../descriptors/Descriptor.hpp"

namespace wde::renderEngine {
	class CoreWindow;

	/**
	 * Class of the Graphics Engine core instance
	 */
	class CoreInstance : NonCopyable {
		public:
			// Class instance
			static CoreInstance& get();


			// Core functions
			/** Initialize the Vulkan instance */
			void initialize();
			/** Clean up the Vulkan instance */
			void cleanUp();
			/** Recreate the full swapchain */
			void recreateSwapChain();

			// Getters and setters
			CoreDevice& getSelectedDevice() { return *_devicesList[_selectedDeviceID]; };
			VkInstance& getInstance() { return _instance; }
			VkSurfaceKHR& getSurface() { return _surface; }
			GLFWwindow* getWindow() { return _window->getWindow(); };
			bool enableValidationLayers() const { return _enableValidationLayers; }
			std::vector<const char *> getValidationLayers() { return _validationLayers; }
			Renderer* getRenderer() const { return _renderer.get(); }
			std::vector<VkFence>& getImagesInFlightFences() { return _inFlightFences; }
			std::vector<VkSemaphore>& getImagesAvailableSemaphores() { return _imageAvailableSemaphores; }
			std::vector<VkSemaphore>& getImagesRenderFinishedSemaphores() { return _renderFinishedSemaphores; }
			std::size_t& getCurrentFrame() { return _currentFrame; }
			const int getMaxFramesInFlight() { return _currentFramesInFlightCount; }
			CoreWindow& getCoreWindow() { return *_window; }
			void addDescriptor(Descriptor* descriptor) {
				_descriptors.push_back(descriptor);
			}

			std::vector<std::unique_ptr<CommandBuffer>>& getCommandBuffers() { return _commandBuffers; }
			std::shared_ptr<CommandPool>& getCommandPool(const std::thread::id &threadID = std::this_thread::get_id()) {
				if (auto it = _commandPools.find(threadID); it != _commandPools.end())
					return it->second;
				return _commandPools.emplace(threadID, std::make_shared<CommandPool>(threadID)).first->second;
			}

			void setFramesInFlightCount(int count) { _currentFramesInFlightCount = count; }
			void setWindow(CoreWindow *window) { _window = window; }
			/**
			 * Sets the current renderer to a new renderer.
			 * @param renderer The new renderer.
			 */
			 void setRenderer(std::unique_ptr<Renderer> &&renderer) { _renderer = std::move(renderer); }


			// Helper functions
			/** Wait for every devices to be ready */
			void waitForDevicesReady();


		private:
            explicit CoreInstance() = default;
			~CoreInstance() = default;


			// References to external objects
			/** The associated GLFW window */
			CoreWindow* _window = nullptr;
			/** The given renderer */
			std::unique_ptr<Renderer> _renderer;
			/** The rendering descriptor list */
			std::vector<Descriptor*> _descriptors {};

			// Vulkan values
			/** A reference to the Vulkan instance */
			VkInstance _instance = VK_NULL_HANDLE;
			/** A reference to the Vulkan surface */
			VkSurfaceKHR _surface = VK_NULL_HANDLE;

			// Devices
			/** A list of every device in the running machine */
			std::vector<std::unique_ptr<CoreDevice>> _devicesList;
			/** Main selected device id in array */
			int _selectedDeviceID = 0;


			// Rendering commands objects
			/** Command pools (one for each thread) */
			std::map<std::thread::id, std::shared_ptr<CommandPool>> _commandPools;
			/** Frames associated command commands */
			std::vector<std::unique_ptr<CommandBuffer>> _commandBuffers;

			// Rendering Sync objects
			/** Max frames being processed at the same time */
			int _currentFramesInFlightCount = 3;
			/** Current drawn frame (% MAX_FRAMES_IN_FLIGHT) */
			std::size_t _currentFrame = 0;
			/** Signals when the corresponding image is done being used by the GPU (CPU-GPU synchronization) */
			std::vector<VkFence> _inFlightFences;
			/** Signals when the corresponding image has been acquired and is ready for rendering */
			std::vector<VkSemaphore> _imageAvailableSemaphores;
			/** Signals when the corresponding image rendering is done, and presentation can happen */
			std::vector<VkSemaphore> _renderFinishedSemaphores;


			// Debug callbacks and layers
			#ifdef NDEBUG // if compile mode = not debug, disable validation layers
					const bool _enableValidationLayers = false;
			#else
					const bool _enableValidationLayers = true;
			#endif
			/** Enabled validation layers (VK_LAYER_KHRONOS_validation = every layer enabled) */
            static const std::vector<const char *> _validationLayers;
			/** The debug messenger callback */
			VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;




			// Setup functions
			/** Create a Vulkan instance */
			void createVulkanInstance();
			/** Create the surface (link between the Window and the instance) */
			void createSurface();
			/** Setup the debug messenger Vulkan's layers callback */
			void setupDebugMessenger();
			/** Setup devices list */
			void setupDevices();
			/** Setup command commands and buffer rendering fences */
			void setupCommandBuffers();



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



	// Helper functions
	/** The debug callback by Vulkan validation layers @return VK_FALSE */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	/** Creates the Vulkan layers Messenger */
	VkResult createDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	/** Destroy the Vulkan layers Messenger */
	void destroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks *pAllocator);
}
