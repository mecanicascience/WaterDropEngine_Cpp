#pragma once

#include <WaterDropEngine/includes.hpp>
#include "DrawSimpleTriangleSubrenderer.hpp"

using namespace wde::renderEngine;

class DrawSimpleTriangleRenderer : public Renderer {
	public:
		/** Set the renderer structure */
		void initialize() override {
			// List of all attachments for the render pass
			std::vector<RenderPassAttachment> renderPassAttachments = {
					// The swapchain render image, initialized using a dark-ish color
					{0, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, wde::Color(0.1f, 0.105f, 0.11f)}
			};

			// List of every sub-passes in the render pass
			std::vector<RenderSubpassType> renderPassSubpasses = {
					// The subpass 0 will write to the swapchain image
					{0, { 0 }}
			};

			// Creates the render pass 0
			this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments, renderPassSubpasses));
		}


		/** Set the sub-renderers */
		void start() override {
			// Use a custom renderer
			this->addSubrenderer<DrawSimpleTriangleSubrenderer>({0, 0});
		}
};
