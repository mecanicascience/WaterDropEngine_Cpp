#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "../../WdeCommon/WdeError/WdeException.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "RendererOld.hpp"
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
			VkPipelineLayout& getPipelineLayout() { return pipelineLayout; }

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
			void drawFrame(CoreWindow &window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, SwapChain &swapChain, VkQueue &graphicsQueue, VkQueue &presentQueue,
			               std::vector<VkFramebuffer> &swapChainFrameBuffers, VkExtent2D &swapChainExtent);


		protected:
			// The struct with pipelines config infos
			struct PipelineConfigInfo {
				VkPipelineVertexInputStateCreateInfo vertexInputInfo;
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
				VkPipelineRasterizationStateCreateInfo rasterizationInfo;
				VkPipelineMultisampleStateCreateInfo multisampleInfo;
				VkPipelineColorBlendAttachmentState colorBlendAttachment;
				VkPipelineColorBlendStateCreateInfo colorBlendInfo;
				VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
				VkPipelineViewportStateCreateInfo viewportInfo;
				std::vector<VkDynamicState> dynamicStateEnables;
				VkPipelineDynamicStateCreateInfo dynamicStateInfo;
			};

			// Core functions
			/** @param configInfo The config infos to modify */
			virtual void setPipelineConfigInfo(PipelineConfigInfo& configInfo) = 0;

			/** @param configInfo The config info to modify */
			virtual void setGraphicsPipelineLayout(VkPipelineLayoutCreateInfo& configInfo) = 0;


			/** @param configInfo The config infos to modify */
			static void setDefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);





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
