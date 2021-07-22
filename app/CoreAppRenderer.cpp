#include "CoreAppRenderer.hpp"

void CoreAppRenderer::initialize() {
	// == Creates the render pass 0 ==
	// List of all attachments for the render pass
	std::vector<RenderPassAttachment> renderPassAttachments0 = {
			{0, "swapchain attachment", RenderPassAttachment::Type::Swapchain, Color(0, 0, 0)} // creates a reference to the swapchain images at the index of 0
	};
	// List of every sub-passes in the render pass
	std::vector<RenderSubpassType> renderPassSubpasses0 = {
			{0, { 0 }} // create a sub-pass that has the attachment number 0 previously declared
	};

	// Creates the render pass 0
	this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
	// ===============================
}

void CoreAppRenderer::start() {
	// Adds a sub render stage to renderer in the render pass 0, and the subpass 0
	this->addSubrenderer<CoreAppSubrenderer>({0, 0});
}
