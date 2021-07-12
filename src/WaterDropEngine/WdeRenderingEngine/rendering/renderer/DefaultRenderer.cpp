#include "DefaultRenderer.hpp"

namespace wde::renderEngine {
	void DefaultRenderer::createRenderPasses(VkCommandBuffer &commandBuffer, VkPipeline &graphicsPipeline, VkRenderPass &renderPass, VkFramebuffer &swapChainFrameBuffer, VkExtent2D &swapChainExtent) {
		// === Starts a render pass ===
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass; // attached render pass
		renderPassInfo.framebuffer = swapChainFrameBuffer; // attached frame buffer

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent; // drawing size

		// Clear colors
		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f}; // black
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


		// === Binds to the graphics pipeline ===
		vkCmdBindPipeline(commandBuffer, // attached command buffers
		                  VK_PIPELINE_BIND_POINT_GRAPHICS, // tell graphics or compute pipeline
		                  graphicsPipeline);


		// === Tell to draw the triangle ===
		vkCmdDraw(commandBuffer,
		          3, // 3 vertices to draw
		          1, // instanced rendering (1 if not doing that)
		          0, // Offset into vertex buffer
		          0); // Offset into instanced rendering


		// === End render pass ===
		vkCmdEndRenderPass(commandBuffer);
	}
}