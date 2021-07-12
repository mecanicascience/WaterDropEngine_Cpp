#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "../../WdeCommon/WdeError/WdeException.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "Renderer.hpp"
#include "../renderObjects/Model.hpp"

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
			virtual ~GraphicsPipeline() {}

			/** Clean up the graphics pipelines */
			void cleanUp();
			void cleanUpPipeline(VkDevice &device);


			/**
			 * Creates the graphic pipelines
			 * @param device
			 * @param swapChain
			 * @param swapChainExtent
			 * @param renderPass
			 */
			void createGraphicsPipeline(VkDevice &device, VkSwapchainKHR &swapChain, VkExtent2D &swapChainExtent, VkRenderPass &renderPass);



			// Getters and setters
			VkPipeline& getPipeline() { return graphicsPipeline; }
			Renderer& getRenderer() { return *renderer; }
			VkRenderPass& getRenderPass() { return renderer->getRenderPass(); }

			void setRenderer(Renderer &renderer) { this->renderer = &renderer; }


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
			// The struct with pipelines config infos
			struct PipelineConfigInfo {
				VkViewport viewport;
				VkRect2D scissor;
				VkPipelineVertexInputStateCreateInfo vertexInputInfo;
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
				VkPipelineRasterizationStateCreateInfo rasterizationInfo;
				VkPipelineMultisampleStateCreateInfo multisampleInfo;
				VkPipelineColorBlendAttachmentState colorBlendAttachment;
				VkPipelineColorBlendStateCreateInfo colorBlendInfo;
				VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
			};

			// Core functions
			/**
			 * @param width The width of the window
			 * @param height The height of the window
			 * @return The corresponding pipelines config infos
			 */
			virtual PipelineConfigInfo getPipelineConfigInfo(uint32_t width, uint32_t height) = 0;

			/**
			 * @param width The width of the window
			 * @param height The height of the window
			 * @return The default pipelines config info
			 */
			static PipelineConfigInfo getDefaultPipelineConfigInfo(uint32_t width, uint32_t height);





		private:
			std::string shaderVertLocation = "undefined";
			std::string shaderFragLocation = "undefined";

			/** Pipeline layout to pass uniform values (dynamic state variables given to pipelines config) */
			VkPipelineLayout pipelineLayout {};
			/** The main created graphic pipelines */
			VkPipeline graphicsPipeline {};


			/** The attached renderers */
			Renderer* renderer {};

			/** True if the pipelines has not been yet destroyed */
			bool pipelineDestroyed = true;


			// Core functions
			/**
			 * @param code The id of the image view buffer
			 * @param device
			 * @return the VkShaderModule from the image view buffer
			 */
			static VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice &device);
	};
}
