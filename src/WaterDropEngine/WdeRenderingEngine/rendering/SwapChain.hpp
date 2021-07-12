#pragma once

#include <sstream>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <memory>

#include "../core/CoreUtils.hpp"
#include "../../WdeCommon/WdeLogger/Logger.hpp"
#include "../../WdeCommon/WdeError/WdeException.hpp"
#include "../core/CoreWindow.hpp"


namespace wde::renderEngine {
	class SwapChain {
		public:
			// Constructors
			SwapChain() = default;
			~SwapChain() = default;

			/** Recreate the swap chain if swap chain no more compatible (like windows size changed) */
			void recreateSwapChain(GLFWwindow *window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface);
			/** CleanUp the frameBuffers */
			void cleanUpFrameBuffers(VkDevice &device);
			/** CleanUp the image views and the swapChain */
			void cleanUpImageViewsAndSwapChain(VkDevice &device);


			// Core functions
			/** Initialize SwapChain linked to the device */
			void initialize(GLFWwindow *window, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, VkDevice &device);
			/** Create the frame buffers already referenced by the render passes */
			void createFrameBuffers(VkRenderPass &renderPass, VkDevice &device);


			// Getters
			VkExtent2D& getSwapChainExtent() { return swapChainExtent; }
			std::vector<VkFramebuffer>& getSwapChainFrameBuffers() { return swapChainFramebuffers; }
			VkSwapchainKHR& getSwapChain() { return swapChain; }
			std::vector<VkImage>& getSwapChainImages() { return swapChainImages; }
			VkFormat& getImageFormat() { return swapChainImageFormat; }



		private:
			/** The swap chain (stores images to be rendered to the GPU for Vulkan) as VkImages in the swapChainImages vector */
			VkSwapchainKHR swapChain;
			/** Frames waiting to be rendered */
			std::vector<VkImage> swapChainImages;
			/** We choose to create 1 image view for each image in the swapChain */
			std::vector<VkImageView> swapChainImageViews;
			/** Holds our framebuffers */
			std::vector<VkFramebuffer> swapChainFramebuffers;

			// Swap chain selected best config
			VkFormat swapChainImageFormat;
			VkExtent2D swapChainExtent;


			// Core functions
			/** Setup the Vulkan swap-chain to store frames given by GLFW and transmit them to window */
			void createSwapChain(GLFWwindow *window, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, VkDevice &device);

			/** Create a basic image view for every image in the swap chain (ex : in 3D, each image
			 *  has 1 image view for the left eye, and one for the right eye. */
			void createImageViews(VkDevice &device);
	};
}
