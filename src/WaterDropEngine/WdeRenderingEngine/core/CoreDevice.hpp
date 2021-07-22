#pragma once

#include <vulkan/vulkan_core.h>

#include "../../../wde.hpp"
#include "CoreWindow.hpp"
#include "CoreDeviceHelper.hpp"
#include "../renderer/passes/SwapChain.hpp"
#include "../renderer/descriptors/RenderStage.hpp"
#include "../renderer/passes/RenderPass.hpp"

namespace wde::renderEngine {
	/**
	 * A unique instance of a corresponding physical device on the machine
	 */
	class CoreDevice : NonCopyable {
		public:
			CoreDevice(int deviceID, CoreWindow &window, VkInstance &instance, VkSurfaceKHR &surface)
				: _deviceID{deviceID}, _window{window}, _instance{instance}, _surface{surface} {};


			// Core functions
			/** Initialize the device */
			void initialize();
			/** Draws the next frame to the screen */
			void draw();
			/** Cleanup the device */
			void cleanUp();


			// Getters and setters
			VkPhysicalDevice& getPhysicalDevice() { return _physicalDevice; }
			VkDevice& getDevice() { return _device; }
			VkQueue& getGraphicsQueue() { return _graphicsQueue; }
			VkQueue& getPresentQueue() { return _presentQueue; }
			SwapChain& getSwapChain() { return _swapchain; }


			// Helpers
			/** @return true is the device supports every required instructions */
			bool isDeviceSuitable() { return CoreDeviceHelper::isDeviceSuitable(_physicalDevice, _deviceExtensions); };

			/**
			 * Starts a new render pass
			 * @param renderPass The corresponding render pass
			 * @return false If the render pass is out of date
			 */
			bool startRenderPass(RenderPass &renderPass);
			/** Ends the given render pass */
			void endRenderPass(RenderPass &renderPass);



		private:
			// References
			CoreWindow& _window;
			/** The Vulkan unique instance */
			VkInstance& _instance;
			/** The Vulkan unique surface */
			VkSurfaceKHR& _surface;

			// WDE values
			/** The ID of the device */
			int _deviceID = 0;
			/** Enabled device extensions (VK_KHR_SWAPCHAIN_EXTENSION_NAME = can GPU displays images) */
			const std::vector<const char *> _deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


			// Device Vulkan values
			/** The corresponding physical device */
			VkPhysicalDevice _physicalDevice = nullptr;
			/** The corresponding logical device */
			VkDevice _device = nullptr;

			/** The swap chain */
			SwapChain _swapchain {};
			/** True if the swapchain should be recreated */
			bool _shouldRecreateSwapchain = false;


			// Queues (VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_SPARSE_BINDING_BIT)
			/** Graphics operations queue of the device (contains drawing points, lines, triangles) */
			VkQueue _graphicsQueue;
			/** Graphics presentation queue of the device (can present infos to the GPU) */
			VkQueue _presentQueue;
	};
}
