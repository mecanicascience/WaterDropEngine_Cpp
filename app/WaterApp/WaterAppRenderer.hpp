#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde;
using namespace wde::renderEngine;
using namespace wde::scene;

class WaterAppRenderer : public Renderer {
	public:
		/** Initialize the renderer */
		void initialize() override {
			// == Creates the render pass 0 ==
			// List of all attachments for the render pass
			Color backgroundColor {0.1f, 0.105f, 0.11f};

			std::vector<RenderPassAttachment> renderPassAttachments0 = {
					// Depth image
					{0, "Depth attachment", RenderPassAttachment::Type::Depth},
					// Land mesh
					{1, "Land mesh", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM, backgroundColor},
					// Swapchain image
					{2, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, backgroundColor}
			};
			// List of every sub-passes in the render pass
			std::vector<RenderSubpassType> renderPassSubpasses0 = {
					{0, { 0, 1 }},
					{1, { 0, 2 }},
					{2, { 2 }}
			};

			// Creates the render pass 0
			this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
		}


		/** Starts the renderer */
		void start() override {
			// Land mesh
			this->addSubrenderer<MeshSubrenderer>({0, 1});

			// Water mesh
			this->addSubrenderer<MeshSubrenderer>({0, 1});

			// Gui sub-renderer
			this->addSubrenderer<GUISubrenderer>({0, 2});
		}
};
