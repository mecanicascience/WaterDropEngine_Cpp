#include "CoreAppRenderer.hpp"
#include "../src/WaterDropEngine/WdeRenderingStructures/objects/models/CubeModel.hpp"

void CoreAppRenderer::initialize() {
	// == Creates the render pass 0 ==
	// List of all attachments for the render pass
	std::vector<RenderPassAttachment> renderPassAttachments0 = {
            // Creates a depth image
            {0, "Depth attachment", RenderPassAttachment::Type::Depth},
            // Creates a reference to the swapchain images at the index of 1
            {1, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, Color(0.01f, 0.01f, 0.01f)}
	};
	// List of every sub-passes in the render pass
	std::vector<RenderSubpassType> renderPassSubpasses0 = {
			// Create a sub-pass that has the attachment number 0 previously declared
			{0, { 0, 1 }}
	};

	// Creates the render pass 0
	this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
	// ===============================
}

void CoreAppRenderer::start() {
	// Adds a sub render stage to renderer in the render pass 0, and the sub-pass 0
	auto cubeSubrenderer = this->addSubrenderer<CoreAppSubrenderer>({0, 0});

	// Set rectangle initial push constants
	auto cube = GameObject::createGameObject();
	cube.model = std::make_shared<CubeModel>();
	cube.transform.translation = {0.0f, 0.0f, 2.5f};
	cube.transform.scale = {0.5f, 0.5f, 0.5f};
	cubeSubrenderer->addGameObject(cube);
}
