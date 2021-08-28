#pragma once

#include <WaterDropEngine/includes.hpp>
using namespace wde::renderEngine;

class DrawSimpleTriangleSubrenderer : public Subrenderer {
	public:
		// Constructor
		/**
		 * Create the sub-renderer at the given render stage
		 * @param stage
		 */
		explicit DrawSimpleTriangleSubrenderer(const RenderStage &stage)
			: Subrenderer(stage),
			 _trianglePipeline(stage, // Rendering pipeline stage
							   {"res/shaders/examples/01_DrawSimpleTriangle/triangle.vert.spv", "res/shaders/examples/01_DrawSimpleTriangle/triangle.frag.spv"}, // Shaders
							   {}, // Hard coded vertices in the shaders
							   PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
							   PipelineGraphics::Depth::None,    // Do not use depth
							   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
							   VK_POLYGON_MODE_FILL,   // Fill drawn shapes
							   VK_CULL_MODE_NONE) {}   // Disable pipeline culling


		// Core functions
		/** Initialize the sub-renderer */
		void initialize() override {
			// Initialize the pipeline
			_trianglePipeline.initialize();
		}

		/** Renders the triangle vertices to the command buffer */
		void render(CommandBuffer &commandBuffer) override {
			// Bind the pipeline to the command buffer
			_trianglePipeline.bind(commandBuffer);

			// Draw 3 hard-coded vertices in the shader
			vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		}


	private:
		/** The rendering graphics pipeline */
		PipelineGraphics _trianglePipeline;
};

