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
			// Only use one subpass
			{0, { 0, 2 }}
	};

	// Creates the render pass 0
	this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
}

void CoreAppRenderer::start() {
	// Mesh sub-renderer
	this->addSubrenderer<MeshSubrenderer>({0, 0});

	// Gui sub-renderer
	this->addSubrenderer<GUISubrenderer>({0, 0});
}
