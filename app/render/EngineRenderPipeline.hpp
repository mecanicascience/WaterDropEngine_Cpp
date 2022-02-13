#pragma once

#include "../../src/WaterDropEngine/WdeRender/render/WdeRenderPipeline.hpp"
#include "../../src/WaterDropEngine/WdeRender/pipelines/PipelineGraphics.hpp"

using namespace wde;
using namespace wde::render;

class EngineRenderPipeline : public WdeRenderPipeline {
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
					std::vector<std::string>{"res/shaders/triangle.vert.spv", "res/shaders/triangle.frag.spv"}, // Shaders
					PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
					PipelineGraphics::Depth::None,    // Do not use depth
					VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
					VK_POLYGON_MODE_FILL,   // Fill drawn shapes
					VK_CULL_MODE_NONE);   // Disable pipeline culling
		}

		void render(CommandBuffer& commandBuffer) override {
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

