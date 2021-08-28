#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde::renderEngine;

class DrawGUIRenderer : public Renderer {
	public:
		/** Set the renderer structure */
		void initialize() override {
			// List of all attachments for the render pass
			std::vector<RenderPassAttachment> renderPassAttachments = {
					// Use depth texture to resolve game object overlapping
					{0, "Depth attachment", RenderPassAttachment::Type::Depth},
					// The swapchain render image, initialized using a dark-ish color
					{1, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, wde::Color(0.1f, 0.105f, 0.11f)}
			};

			// List of every sub-passes in the render pass
			std::vector<RenderSubpassType> renderPassSubpasses = {
					// The subpass 0 will draw game objects to the swapchain image, and use the depth texture
					{0, { 0, 1 }},
					// The subpass 1 will draw GUI to the swapchain image
					{1, { 1 }}
			};

			// Creates the render pass 0
			this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments, renderPassSubpasses));
		}


		/** Set the sub-renderers */
		void start() override {
			// Render meshes at render-pass 0-0
			this->addSubrenderer<MeshSubrenderer>({0, 0});

			// Draw GUI at render-pass 0-1
			this->addSubrenderer<GUISubrenderer>({0, 1});
		}
};

