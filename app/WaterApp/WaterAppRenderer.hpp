#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde;
using namespace wde::renderEngine;
using namespace wde::scene;

class WaterAppRenderer : public Renderer {
	public:
		/** Initialize the renderer */
		void initialize() override {
			std::vector<RenderPassAttachment> renderPassAttachments0 = {
					// Depth image for land
					{0, "Depth attachment for land depth", RenderPassAttachment::Type::Depth},
					// Land depth image
					{1, "Land depth", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM},
					// Land depth normals
					{2, "Land normals", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM},
					// Land depth texture
					{3, "Land texture", RenderPassAttachment::Type::Image, VK_FORMAT_R8G8B8A8_UNORM},
					// Swapchain image
					{4, "Swapchain attachment", RenderPassAttachment::Type::Swapchain, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
			};
			std::vector<RenderSubpassType> renderPassSubpasses0 = {
					// Render terrain depth mesh
					{ 0, { 0, 1 }},
					// Render terrain normals mesh
					{ 1, { 0, 2 }},
					// Render terrain texture
					{2, { 0, 3 }},
					// Render terrain texture to swapchain
					{3, { 0, 4 }},
					// Draw water
					{4, { 0, 4 }, { 1, 2, 3 }},
					// Render GUI
					{5, { 4 }}
			};
			this->addRenderPass(std::make_unique<RenderPass>(renderPassAttachments0, renderPassSubpasses0));
		}


		/** Starts the renderer */
		void start() override {
			// Render terrain depth texture
			this->addSubrenderer<MeshSubrenderer>({0, 0});
			// Render terrain normals texture
			this->addSubrenderer<MeshSubrenderer>({0, 1});
			// Render terrain color
			this->addSubrenderer<MeshSubrenderer>({0, 2});
			// Render terrain color to swapchain
			this->addSubrenderer<MeshSubrenderer>({0, 3});
			// Draw water
			this->addSubrenderer<MeshSubrenderer>({0, 4});
			// Render GUI
			this->addSubrenderer<GUISubrenderer>({0, 5});
		}
};
