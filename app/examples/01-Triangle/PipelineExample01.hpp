#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeRender/pipelines/PipelineGraphics.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample01 : public WdeRenderPipelineInstance {
		std::unique_ptr<PipelineGraphics> _trianglePipeline;

		public:
			void setup() override {
				// Create passes attachments
				setAttachments({
					{0, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
				});

				// Create passes and subpasses structure
				setStructure({
					{0, {
						{0, {}, { 0 }},
						{1, {}, { 0 }} // GUI Pass
					}}
				});

				// Initialize GUI
				gui::WdeGUI::initialize(std::pair<int, int>{0, 1});

				// Create pipeline
				_trianglePipeline = std::make_unique<PipelineGraphics>(
						std::pair<int, int>{0, 0},
						std::vector<std::string>{
							"res/shaders/examples/01-Triangle/triangle.vert",
							"res/shaders/examples/01-Triangle/triangle.frag"
						}, // Shaders
                        std::vector<scene::VertexInput>{}, // No vertices provided
						PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
						PipelineGraphics::Depth::None,    // Do not use depth
						VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
						VK_POLYGON_MODE_FILL,   // Fill drawn shapes
						VK_CULL_MODE_NONE);   // Disable pipeline culling
				_trianglePipeline->initialize();
			}

			void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override {
				beginRenderPass(0);
					beginRenderSubPass(0);
						// Bind the pipeline to the command buffer
						_trianglePipeline->bind(commandBuffer);

						// Draw 3 hard-coded vertices in the shader
						vkCmdDraw(commandBuffer, 3, 1, 0, 0);
					endRenderSubPass();

					beginRenderSubPass(1);
						// Render GUI
						gui::WdeGUI::render(commandBuffer);
					endRenderSubPass();
				endRenderPass();
			}


			void cleanUp() override {
				_trianglePipeline.reset();
			}
	};
}
