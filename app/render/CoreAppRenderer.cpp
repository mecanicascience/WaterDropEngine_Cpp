#include "CoreAppRenderer.hpp"

void CoreAppRenderer::initialize() {
	// == Creates the render pass 0 ==
	// List of all attachments for the render pass
	std::vector<RenderPassAttachment> renderPassAttachments0 = {
            // Depth image
            {0, "Depth attachment", RenderPassAttachment::Type::Depth},
            // Mesh image
            {1, "Mesh image", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM},
            // Filtered image
            {2, "Filter image", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM},
            // Render to swapchain
            {3, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
	};
	// List of every sub-passes in the render pass
	std::vector<RenderSubpassType> renderPassSubpasses0 = {
			{0, { 0, 1 }},
			{1, { 2 }, { 1 }},
			{2, { 3 }, { 2 }},
			{3, { 3 }}
	};

	// Creates the render pass 0
	this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
}


void CoreAppRenderer::start() {
	// Mesh sub-renderer
	this->addSubrenderer<MeshSubrenderer>({0, 0});

	this->addSubrenderer<InvertFilter>({0, 1}, 1);
	this->addSubrenderer<DefaultFilter>({0, 2}, 2);

	// Gui sub-renderer
	this->addSubrenderer<GUISubrenderer>({0, 3});
}
