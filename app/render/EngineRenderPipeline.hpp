#pragma once

#include "../../src/WaterDropEngine/WdeRender/render/WdeRenderPipeline.hpp"

using namespace wde;
using namespace wde::render;

class EngineRenderPipeline : public WdeRenderPipeline {
	void setup() override {
		// Create passes attachments
		setAttachments({
			{0, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)},
			{1, "Depth attachment", RenderAttachment::DEPTH},
			{2, "Image attachment", RenderAttachment::IMAGE, VK_FORMAT_R16_UNORM}
		});

		// Create passes and subpasses structure
		setStructure({
			{0, {
				{0, { }, { 2 }},
				{1, { 2 }, { 0, 1 }}
			}}
        });
	}

	void render(CommandBuffer& commandBuffer) override {
		beginRenderPass(0);
			beginRenderSubPass(0);
			endRenderSubPass();

			beginRenderSubPass(1);
			endRenderSubPass();
		endRenderPass();
	}
};

