#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

#include "../../core/CoreUtils.hpp"
#include "../../../WdeCommon/WdeError/WdeException.hpp"
#include "../SwapChain.hpp"

namespace wde::renderEngine {
	class Renderer {
		public:
			// Constructors
			Renderer() {}
			virtual ~Renderer() {}

			/** Clean up and destroy renderer */
			void cleanUp(VkDevice &device);
			/** Clean up the command buffers */
			void cleanUpCommandBuffers(VkDevice &device);

			// Core functions
			/**
			 * Initialize the Renderer
			 * @param physicalDevice
			 * @param device
			 * @param swapChain
			 * @param surface
			 * @param graphicsPipeline
			 */
			void initialize(VkPhysicalDevice &physicalDevice, VkDevice &device, VkSurfaceKHR &surface, VkRenderPass &renderPass,
			                VkPipeline &graphicsPipeline, std::vector<VkFramebuffer> &swapChainFrameBuffers, VkExtent2D &swapChainExtent, std::vector<VkImage>& swapChainImages);


			/**
			 * Creates the render passes of the Renderer
			 * @param device
			 * @param swapChain
			 */
			void createRenderPasses(VkDevice &device, VkFormat &swapChainImageFormat);


			/**
			 * Draws the next frame
			 * @param window
			 * @param device
			 * @param physicalDevice
			 * @param surface
			 * @param swapChain
			 * @param graphicsQueue
			 * @param presentQueue
			 */
			void drawFrame(CoreWindow &window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, SwapChain &swapChain, VkQueue &graphicsQueue, VkQueue &presentQueue);


			/**
			 * Initialize the command buffers (this->commandBuffers)
			 * @param device
			 * @param graphicsPipeline
			 * @param swapChainFrameBuffers
			 * @param swapChainExtent
			 * @param renderPass
			 */
			void createCommandBuffers(VkDevice &device, VkPipeline &graphicsPipeline, std::vector<VkFramebuffer> &swapChainFrameBuffers, VkExtent2D &swapChainExtent, VkRenderPass &renderPass);


			// Getter and setters
			size_t& getCurrentFrame() { return currentFrame; }
			bool shouldRecreateSwapChain() { return shouldRecreateSwapChainBool; }
			void setShouldRecreateSwapChain(bool val) { shouldRecreateSwapChainBool = val; }
			std::vector<VkFence>& getImagesInFlight() { return imagesInFlight; }
			VkRenderPass& getRenderPass() { return renderPass; }


		protected:
			/**
			 * Create render passes
			 * @param commandBuffer
			 * @param graphicsPipeline
			 * @param renderPass
			 * @param swapChainFrameBuffer
			 * @param swapChainExtent
			 */
			virtual void createRenderPasses(VkCommandBuffer &commandBuffer, VkPipeline &graphicsPipeline, VkRenderPass &renderPass, VkFramebuffer &swapChainFrameBuffer, VkExtent2D &swapChainExtent) = 0;


		private:
			/** Max frames count that can be processed at the same time */
			const int MAX_FRAMES_IN_FLIGHT = 2;

			/** Recreate a swap chain if this value is true */
			bool shouldRecreateSwapChainBool;

			/** The swap-chain render pass */
			VkRenderPass renderPass;


			/** Manage memory that is used to store buffers - command buffers allocated from them */
			VkCommandPool commandPool;
			/** The command buffers (one for each VkFrameBuffer - auto submits to command queues for processing) */
			std::vector<VkCommandBuffer> commandBuffers;


			/** Semaphores for when an image is available (id = frame id to be rendered) */
			std::vector<VkSemaphore> imageAvailableSemaphores;
			/** Semaphores for when an image is done rendering (id = frame id to be rendered) */
			std::vector<VkSemaphore> renderFinishedSemaphores;
			/** Fences for when GPU is ready to be called by CPU (we wait for fences to continue) */
			std::vector<VkFence> inFlightFences;
			/** Fences for which swap chain images are already being processed */
			std::vector<VkFence> imagesInFlight;

			/** The current frame for the semaphores to use right pair */
			size_t currentFrame = 0;



			// Core functions
			/** Creates the command pool (this->commandPool) */
			void createCommandPool(VkPhysicalDevice &physicalDevice, VkDevice &device, VkSurfaceKHR &surface);

			/** Create semaphores for operations sync */
			void createSyncObjects(VkDevice &device, std::vector<VkImage>& swapChainImages);
	};
}
