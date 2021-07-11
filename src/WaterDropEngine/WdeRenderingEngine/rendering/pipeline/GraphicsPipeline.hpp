#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "../../../WdeCommon/WdeError/WdeException.hpp"
#include "../../../WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "../Renderer.hpp"

namespace wde::renderEngine {
	class SwapChain;

	class GraphicsPipeline {
		public:
			/**
			 * @param shaderVertLocation The local path of the vertex shader
             * @param shaderFragLocation The local path of the fragment shader
			 */
			GraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation)
				: shaderVertLocation(shaderVertLocation), shaderFragLocation(shaderFragLocation), renderer() { };
			virtual ~GraphicsPipeline() = default;
			void cleanUp(VkDevice &device);


			/**
			 * Destroy the pipeline
			 * @param device
			 */
			void destroyPipeline(VkDevice &device);

			/**
			 * Creates the graphic pipeline
			 * @param device
			 * @param swapChain
			 * @param swapChainExtent
			 * @param renderPass
			 */
			virtual void createGraphicsPipeline(VkDevice &device, VkSwapchainKHR &swapChain, VkExtent2D &swapChainExtent, VkRenderPass &renderPass) = 0;



			/**
			 * Creates the render passes of the Renderer
			 * @param device
			 * @param swapChain
			 */
			void createRenderPasses(VkDevice &device, VkFormat &swapChainImageFormat);


			// Getters and setters
			VkPipeline& getPipeline() { return graphicsPipeline; }
			Renderer& getRenderer() { return renderer; }
			VkRenderPass& getRenderPass() { return renderPass; }


			/**
			 * Draw the next frame to the screen
			 * @param window
			 * @param device
			 * @param physicalDevice
			 * @param surface
			 * @param swapChain
			 * @param graphicsQueue
			 * @param presentQueue
			 */
			void drawFrame(CoreWindow &window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, SwapChain &swapChain, VkQueue &graphicsQueue, VkQueue &presentQueue);


	protected:
			std::string &shaderVertLocation;
			std::string &shaderFragLocation;

			/** Pipeline layout to pass uniform values (dynamic state variables given to pipeline config) */
			VkPipelineLayout pipelineLayout {};
			/** The main created graphic pipeline */
			VkPipeline graphicsPipeline {};
			/** The swap-chain render pass */
			VkRenderPass renderPass;

			/** The attached renderer */
			Renderer renderer;

			/** True if the pipeline has not been yet destroyed */
			bool pipelineDestroyed = true;



			// Helper functions
			/**
			 * Creates the default graphics pipeline
			 * @param device
			 * @param swapChain
			 * @param swapChainExtent
			 * @param renderPass
			 */
			void createDefaultGraphicsPipeline(VkDevice &device, VkSwapchainKHR &swapChain, VkExtent2D &swapChainExtent, VkRenderPass &renderPass);

			/**
			 * @param code The id of the image view buffer
			 * @param device
			 * @return the VkShaderModule from the image view buffer
			 */
			static VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice &device);
	};
}
