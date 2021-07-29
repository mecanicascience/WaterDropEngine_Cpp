#include "CoreAppRenderer.hpp"

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
			{0, { 0, 1 }} // create a sub-pass that has the attachment number 0 previously declared
	};

	// Creates the render pass 0
	this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
	// ===============================
}

void CoreAppRenderer::start() {
	// Adds a sub render stage to renderer in the render pass 0, and the sub-pass 0
	auto squareSubrenderer = this->addSubrenderer<CoreAppSubrenderer>({0, 0});


	// Create corresponding rectangles
	for (int i = 0; i < 10; i++) {
		// Create a simple rectangle model
		std::vector<Model::Vertex> vertices = {
				{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
				{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
				{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};
		std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
		auto rectangleModel = std::make_shared<Model>(vertices, indices);

		// Set rectangle initial push constants
		auto rectangle = GameObject::createGameObject();
		rectangle.model = rectangleModel;
		rectangle.transform2D.translation.x = 0.2f;
		rectangle.transform2D.scale.y = 1.0f;
		rectangle.transform2D.rotation = 0.25f * 3.14125f;
		squareSubrenderer->addGameObject(rectangle);
	}
}
