#pragma once

#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

#include "WdeWindow.hpp"
#include "../Constants.hpp"

namespace wde {
    /** Struct that handles any type of Queue */
    struct QueueFamilyIndices {
        uint32_t graphicsFamily {};
        uint32_t presentFamily {};
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;

        bool isComplete() {
            return graphicsFamilyHasValue && presentFamilyHasValue;
        }
    };

    /** Struct for the swap-chain (stores images given by external api to a buffer) */
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    /**
     * Handles Vulkan and JWGL link, debug layers and error outputs & physical devices & logical devices configuration<br />
     * See <a href="https://gpuopen.com/learn/understanding-vulkan-objects/">https://gpuopen.com/learn/understanding-vulkan-objects/</a>
     * for more information on how everything works.
     */
    class WdeInstanceDevice {
        public :
            #ifdef NDEBUG // if compile mode = not debug, disable validation layers
                    const bool enableValidationLayers = false;
            #else
                    const bool enableValidationLayers = true;
            #endif

            WdeInstanceDevice(WdeWindow &window);
            ~WdeInstanceDevice();

            // Not copyable or movable
            WdeInstanceDevice(const WdeInstanceDevice &) = delete;
            void operator=(const WdeInstanceDevice &) = delete;
            WdeInstanceDevice(WdeInstanceDevice &&) = delete;
            WdeInstanceDevice &operator=(WdeInstanceDevice &&) = delete;

            /** Initialize Vulkan device */
            void initialize();


        private :
            WdeWindow &window;
            VkInstance instance{};

            /** Enabled validation layers (VK_LAYER_KHRONOS_validation = every layer enabled) */
            const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
            /** Enabled device extensions (VK_KHR_SWAPCHAIN_EXTENSION_NAME = can GPU displays images) */
            const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

            /** The debug messenger callback */
            VkDebugUtilsMessengerEXT debugMessenger;

            /** Selected graphic card */
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            /** Interface between the Window and the physical device */
            VkDevice device;
            /** Vulkan representation of the Window (pushes graphics to the swap-chain) */
            VkSurfaceKHR surface;

            /** Graphics operations queue passed to the physical device */
            VkQueue graphicsQueue;
            /** Graphics presentation queue passed to the physical device (allows device to present images to surface) */
            VkQueue presentQueue;


            // Main config functions
            /** Create a Vulkan instance linked to the JWGL context */
            void createInstance(const uint32_t& appVersion, const std::string& appName);

            /** Setup the debug messenger Vulkan's layers callback */
            void setupDebugMessenger();

            /** Create the surface between the window and Vulkan */
            void createSurface();

            /** Select graphics card that support Vulkan */
            void pickPhysicalDevice();

            /** Setup the logical device to interact with the physical device */
            void createLogicalDevice();

            /** Setup the Vulkan swap-chain to store frame data given by GLFW */
            void createSwapChain();



            // Helper functions
            /** @return a list of the extensions required by Vulkan */
            std::vector<const char *> getRequiredExtensions();

            /** Check if every required extension are available and loaded (avoid VK_ERROR_EXTENSION_NOT_PRESENT) */
            void hasRequiredExtensions();

            /**
             * @param device
             * @return true if the device supports the required parameters
             */
            bool isDeviceSuitable(VkPhysicalDevice device);



            /** @return true if every required validation (= debug) layer are available */
            bool checkValidationLayerSupport();

            /**
             * @param device
             * @return Return true if the device support the required extensions
             */
            bool checkDeviceExtensionSupport(VkPhysicalDevice device);



            /**
             * Update the debug messenger layers struct
             * @param createInfo The struct that needs to be updated
             */
            static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

            /**
             * @param device
             * @return The list of queues supported by the device
             */
            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

            /**
             * Get the swap chain details of the given device
             * @param device The device
             * @return The details of the swap chain of the device
             */
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
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
