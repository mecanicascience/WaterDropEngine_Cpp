#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde;
using namespace wde::renderEngine;
using namespace wde::scene;

class ComputeAppRenderer : public Renderer {
	public:
		/** Initialize the renderer */
		void initialize() override {
			std::vector<RenderPassAttachment> renderPassAttachments0 = {
					// Depth image for water
					{0, "Depth attachment", RenderPassAttachment::Type::Depth},
					// Swapchain image
					{1, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
			};
			std::vector<RenderSubpassType> renderPassSubpasses0 = {
					// Render mesh
					{ 0, { 0, 1 }},
					// Render GUI
					{1, { 1 }}
			};
			this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
		}


		/** Starts the renderer */
		void start() override {
			// Draw water
			this->addSubrenderer<MeshSubrenderer>({0, 0});
			// Render GUI
			this->addSubrenderer<GUISubrenderer>({0, 1});
		}
};
