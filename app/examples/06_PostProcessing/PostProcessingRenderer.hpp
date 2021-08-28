#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde::renderEngine;

class PostProcessingRenderer : public Renderer {
	public:
		/** Set the renderer structure */
		void initialize() override {
			// List of all attachments for the render pass
			std::vector<RenderPassAttachment> renderPassAttachments = {
					// Use depth texture to resolve game object overlapping
					{0, "Depth attachment", RenderPassAttachment::Type::Depth},
					// The image where the meshes will be rendered to
					{1, "Mesh image", RenderPassAttachment::Type::Image},
					// The swapchain render image, initialized using a dark-ish color
					{2, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, wde::Color(0.1f, 0.105f, 0.11f)}
			};

			// List of every sub-passes in the render pass
			std::vector<RenderSubpassType> renderPassSubpasses = {
					// The subpass 0 will write to the mesh image and use the depth texture to resolve overlapping
					{0, { 0, 1 }},
					// Subpass 1 will read from the mesh image, and write it with inverted colors on the swapchain image
					{1, { 2 }, { 1 }}
			};

			// Creates the render pass 0
			this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments, renderPassSubpasses));
		}


		/** Set the sub-renderers */
		void start() override {
			// Render meshes at render-pass 0-0
			this->addSubrenderer<MeshSubrenderer>({0, 0});

			// Invert swapchain image colors (swapchain color attachment is at binding 1)
			this->addSubrenderer<InvertFilter>({0, 1}, 1);
		}
};

