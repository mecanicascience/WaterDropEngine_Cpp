#pragma once

#include "GraphicsPipeline.hpp"

namespace wde::renderEngine {
	class GraphicsDefaultPipeline : public GraphicsPipeline {
		public:
			GraphicsDefaultPipeline(std::string shaderVertLocation, std::string shaderFragLocation) : GraphicsPipeline(shaderVertLocation, shaderFragLocation) { };

			/**
			 * Creates a default graphics pipeline (auto called when referenced)
			 * @param device
			 * @param swapChain
			 * @param swapChainExtent
			 * @param renderPass
			 */
			void createGraphicsPipeline(VkDevice &device, VkSwapchainKHR &swapChain, VkExtent2D &swapChainExtent, VkRenderPass &renderPass) override;
	};
}
