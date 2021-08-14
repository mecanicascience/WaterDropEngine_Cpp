#include "CoreAppRenderer.hpp"

void CoreAppRenderer::initialize() {
	// == Creates the render pass 0 ==
	// List of all attachments for the render pass
	std::vector<RenderPassAttachment> renderPassAttachments0 = {
            // Depth image
            {0, "Depth attachment", RenderPassAttachment::Type::Depth},
            // Mesh image
            {1, "Mesh image", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM, Color(0.1f, 0.105f, 0.11f)},
            // Render to swapchain
            {2, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
	};
	// List of every sub-passes in the render pass
	std::vector<RenderSubpassType> renderPassSubpasses0 = {
			{0, { 0, 1 }},
			{1, { 2 }, { 1 }},
			{2, { 2 }}
	};

	// Creates the render pass 0
	this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
}


void CoreAppRenderer::start() {
	// Mesh sub-renderer
	this->addSubrenderer<MeshSubrenderer>({0, 0});

	// Filter
	// this->addSubrenderer<InvertFilter>({0, 1}, 1);
	this->addSubrenderer<DefaultFilter>({0, 1}, 1);
	// this->addSubrenderer<DepthViewerFilter>({0, 1}, 0); // Displays real depth buffer

	// Gui sub-renderer
	this->addSubrenderer<GUISubrenderer>({0, 2});
}
