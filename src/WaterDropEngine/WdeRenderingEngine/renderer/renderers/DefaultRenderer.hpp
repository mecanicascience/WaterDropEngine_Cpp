#pragma once

#include "../Renderer.hpp"

namespace wde::renderEngine {
	class DefaultRenderer : public Renderer {
		public:
			// Constructors
			DefaultRenderer() = default;
			~DefaultRenderer() = default;

		protected:
			// Core functions
			void createRenderPasses(Model& model, VkCommandBuffer &commandBuffer, VkPipeline &graphicsPipeline, VkRenderPass &renderPass, VkFramebuffer &swapChainFrameBuffer, VkExtent2D &swapChainExtent) override;
	};
}
