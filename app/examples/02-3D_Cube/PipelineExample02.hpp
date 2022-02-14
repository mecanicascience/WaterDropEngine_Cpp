#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample02 : public WdeRenderPipelineInstance {
		public:
			void setup() override {
				// Create passes attachments
				setAttachments({
					{0, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
				});

				// Create passes and subpasses structure
				setStructure({
					{0, {
						{0, {}, { 0 }}
					}}
				});
			}

			void render(CommandBuffer& commandBuffer) override {
				beginRenderPass(0);
					beginRenderSubPass(0);


					endRenderSubPass();
				endRenderPass();
			}


			void cleanUp() override { }
	};
}
