#include "CoreAppRenderer.hpp"

void CoreAppRenderer::initialize() {
	// == Creates the render pass 0 ==
	// List of all attachments for the render pass
	std::vector<RenderPassAttachment> renderPassAttachments0 = {
            // Depth image
            {0, "Depth attachment", RenderPassAttachment::Type::Depth},
            // Filter image
            {1, "Filter image", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM},
            // Render to swapchain
            {2, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, Color(0.01f, 0.01f, 0.01f)}
	};
	// List of every sub-passes in the render pass
	std::vector<RenderSubpassType> renderPassSubpasses0 = {
			// Apply small filter
			{0, { 0, 2 }}
	};

	// Creates the render pass 0
	this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
}

void CoreAppRenderer::start() {
	// == Render pass 0-0 ==
	// Adds a sub render stage to renderer in the render pass 0, and the sub-pass 0
	auto cubeSubrenderer = this->addSubrenderer<CoreAppSubrenderer>({0, 0});

	// Set rectangle initial push constants
	/*auto cube = GameObject::createGameObject();
	cube.model = std::make_shared<ModelCube>();
	cube.transform.translation = {0.0f, 0.0f, 2.5f};
	cube.transform.scale = {0.5f, 0.5f, 0.5f};
	cubeSubrenderer->addGameObject(cube);*/

	auto testGO = GameObject::createGameObject();
	testGO.model = std::make_shared<ModelLoader>("res/models/monkey_smooth.obj");
	testGO.transform.scale /= 5.0f;
	cubeSubrenderer->addGameObject(testGO);

	// Gui sub-renderer
	this->addSubrenderer<GUISubrenderer>({0, 0});
}
