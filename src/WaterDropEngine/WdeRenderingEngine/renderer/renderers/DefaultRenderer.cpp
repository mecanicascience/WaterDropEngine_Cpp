#include "DefaultRenderer.hpp"

namespace wde::renderEngine {
	void DefaultRenderer::createRenderPasses(Model& model, VkCommandBuffer &commandBuffer, VkPipeline &graphicsPipeline, VkRenderPass &renderPass, VkFramebuffer &swapChainFrameBuffer, VkExtent2D &swapChainExtent) {
		// === Starts a render pass ===
		VkRenderPassBeginInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass; // attached render pass
		renderPassInfo.framebuffer = swapChainFrameBuffer; // attached frame buffer

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent; // drawing size

		// Clear colors
		std::array<VkClearValue, 2> clearValues{};
		// index 0 = color attachment, index 1 = depth attachment
		clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		// Start renderer pass
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


		// === Binds to the graphics pipelines ===
		vkCmdBindPipeline(commandBuffer, // attached command buffers
		                  VK_PIPELINE_BIND_POINT_GRAPHICS, // tell graphics or compute pipelines
		                  graphicsPipeline);

		// Bind the models to the command buffer
		model.bind(commandBuffer);
		model.draw(commandBuffer);


		// === End render pass ===
		vkCmdEndRenderPass(commandBuffer);
	}
}
