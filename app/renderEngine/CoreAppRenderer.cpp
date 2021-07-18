#include <glm/gtc/constants.hpp>
#include "CoreAppRenderer.hpp"
#include "../CoreApp.hpp"



void CoreAppRenderer::cleanUp(VkDevice &device) {
	for (auto& obj : gameObjects) {
		obj.model->cleanUp(device);
	}
}




void CoreAppRenderer::loadGameObjects(VkDevice &device, VkPhysicalDevice &physicalDevice) {
	// Create Model data
	std::vector<Model::Vertex> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };

	// Create model
	auto model = std::make_shared<Model>(device, physicalDevice, vertices, indices);

	// Create game objects
	auto rectangle = GameObject::createGameObject();
	rectangle.model = model;
	rectangle.color = {0.1f, 0.8f, 0.1f};
	rectangle.transform2d.translation.x = 0.2f;
	rectangle.transform2d.scale = {2.0f, 0.5f};
	rectangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

	gameObjects.push_back(std::move(rectangle));
}

void CoreAppRenderer::renderGameObjects(VkCommandBuffer &commandBuffer, VkPipelineLayout &pipelineLayout) {
	for (auto& obj : gameObjects) {
		obj.transform2d.rotation += 0.005;

		CoreApp::SimplePushConstantData push {};
		push.offset = obj.transform2d.translation;
		push.color = obj.color;
		push.transform = obj.transform2d.mat2();

		vkCmdPushConstants(
				commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0, sizeof(CoreApp::SimplePushConstantData), &push);
		obj.model->bind(commandBuffer);
		obj.model->draw(commandBuffer);
	}
}



void CoreAppRenderer::createRenderPasses(VkCommandBuffer &commandBuffer, VkPipeline &graphicsPipeline, VkPipelineLayout &pipelineLayout, VkRenderPass &renderPass, VkFramebuffer &swapChainFrameBuffer, VkExtent2D &swapChainExtent) {
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

	// Update viewport and scissor size
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor {{0, 0}, swapChainExtent};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


	// Binds the command buffer to the graphics pipeline
	vkCmdBindPipeline(commandBuffer, // attached command buffers
	                  VK_PIPELINE_BIND_POINT_GRAPHICS, // tell graphics or compute pipelines
	                  graphicsPipeline);

	// Render commands
	renderGameObjects(commandBuffer, pipelineLayout);

	// === End render pass ===
	vkCmdEndRenderPass(commandBuffer);
}
