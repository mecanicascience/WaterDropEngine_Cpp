#pragma once

#include "../../src/WaterDropEngine/WdeRenderingEngine/renderer/Renderer.hpp"

class CoreAppRenderer : public wde::renderEngine::Renderer {
	public:
		// Constructors
		CoreAppRenderer() = default;
		~CoreAppRenderer() = default;

	protected:
		// Core functions
		void createRenderPasses(wde::renderEngine::Model& model, VkCommandBuffer &commandBuffer, VkPipeline &graphicsPipeline, VkPipelineLayout &pipelineLayout, VkRenderPass &renderPass, VkFramebuffer &swapChainFrameBuffer, VkExtent2D &swapChainExtent) override;
};
