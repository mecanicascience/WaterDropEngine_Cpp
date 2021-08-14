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
					// Land normals
					{1, "Land normal", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM, backgroundColor},
					// Land depth
					{2, "Land depth", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM},
					// Swapchain image
					{3, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, backgroundColor}
			};
			// List of every sub-passes in the render pass
			std::vector<RenderSubpassType> renderPassSubpasses0 = {
					// Draw land normal map
					{0, { 1 }},
					// Compute land depth
					{1, { 2 }},
					// Draw land mesh texture to the swapchain
					{2, { 0, 3 }},
					// Draw water
					{3, { 0, 3 }, { 1, 2 }},
					// Draw GUI
					{4, { 3 }}
			};

			// Creates the render pass 0
			this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
		}


		/** Starts the renderer */
		void start() override {
			// Draw land normal map
			this->addSubrenderer<MeshSubrenderer>({0, 0});

			// Compute land depth
			this->addSubrenderer<MeshSubrenderer>({0, 1});

			// Draw land mesh to the swapchain
			this->addSubrenderer<MeshSubrenderer>({0, 2});

			// Draw water
			this->addSubrenderer<MeshSubrenderer>({0, 3});

			// Draw GUI
			this->addSubrenderer<GUISubrenderer>({0, 4});
		}
};
