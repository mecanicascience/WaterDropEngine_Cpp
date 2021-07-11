#include "GraphicsDefaultPipeline.hpp"

namespace wde::renderEngine {
	void GraphicsDefaultPipeline::createGraphicsPipeline(VkDevice &device, VkSwapchainKHR &swapChain, VkExtent2D &swapChainExtent, VkRenderPass &renderPass) {
		// Creates the main graphics pipeline
		createDefaultGraphicsPipeline(device, swapChain, swapChainExtent, renderPass);
	}
}
