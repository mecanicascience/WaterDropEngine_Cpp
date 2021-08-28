#pragma once

#include <WaterDropEngine/includes.hpp>
using namespace wde::renderEngine;

/**
 * Creates a chessboard image using a compute shader, and displays it on the screen
 */
class ComputeShaderSubrenderer : public Subrenderer {
	public:
		// Constructor
		/**
		 * Create the sub-renderer at the given render stage
		 * @param stage
		 */
		explicit ComputeShaderSubrenderer(const RenderStage &stage)
			: Subrenderer(stage),
			  // Create the compute pipeline with the adequate compute shader
			  _computePipeline("res/shaders/examples/05_ComputeShader/computeShaderChessboard.comp.spv"),
			  // Create a simple graphics pipeline with no culling, and that will display the texture to the screen
			  _textureDisplayPipeline({0, 0},
									  {"res/shaders/examples/05_ComputeShader/textureDisplayer.vert.spv", "res/shaders/examples/05_ComputeShader/textureDisplayer.frag.spv"},
									  {}, PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::ReadWrite,
									VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
									VK_CULL_MODE_NONE) {}


		// Core functions
		/** Initialize the sub-renderer */
		void initialize() override {
			// == INIT ==
			// Create a 200px*200px texture where the compute shader will write, and where the texture displayer will read from
			_chessboardImage = std::make_shared<Texture2D>(VkExtent2D{300, 300}, // The image will be scaled up to fullscreen by the displaying shader
														   VK_FORMAT_R8G8B8A8_UNORM, // Image format compatible with compute shaders
														   VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT); // Use texture in compute shaders, and as a sampled image


			// Create compute shader descriptor (describes objects used in the shader)
			_computeDescriptor = std::make_shared<Descriptor>();
			_computeDescriptor->addSet(0, {
				{0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, _chessboardImage->getView(), _chessboardImage->getSampler(), VK_SHADER_STAGE_COMPUTE_BIT}
			});
			// Initialize the compute pipeline
			_computePipeline.setDescriptor(_computeDescriptor);
			_computePipeline.initialize();


			// Create texture displaying shader descriptor
			_textureDisplayDescriptor = std::make_shared<Descriptor>();
			_textureDisplayDescriptor->addSet(0, {
				{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _chessboardImage->getView(), _chessboardImage->getSampler()}
			});
			// Initialize the displaying texture pipeline
			_textureDisplayPipeline.setDescriptor(_textureDisplayDescriptor);
			_textureDisplayPipeline.initialize();



			// == RUN ==
			// Transition image to shader writing layout, so that the image can now be written by the compute shader
			_chessboardImage->toLayout(VK_IMAGE_LAYOUT_GENERAL);

			// Create a temporary command buffer and bind it to the pipeline
			CommandBuffer commandBuffer {true};
			_computePipeline.bind(commandBuffer);

			// Run the compute shader (use 32*32*1 groups in this demo)
			_computePipeline.bind(_computeDescriptor);
			vkCmdDispatch(commandBuffer, 32, 32, 1);

			// Submit the command buffer
			commandBuffer.end();
			commandBuffer.submit();
			commandBuffer.waitForQueueIdle();

			// Transition image to shader reading layout, so that the image can now be read inside the shaders
			_chessboardImage->toLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		/** Renders the texture vertices to the command buffer */
		void render(CommandBuffer &commandBuffer) override {
			// Bind the command buffer to the texture displaying pipeline
			_textureDisplayPipeline.bind(commandBuffer);
			_textureDisplayPipeline.bind(_textureDisplayDescriptor);

			// Display the textured image
			vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		}


	private:
		// The image that will be created then displayed
		std::shared_ptr<Texture2D> _chessboardImage {};

		// The compute pipeline requirements
		PipelineCompute _computePipeline;
		std::shared_ptr<Descriptor> _computeDescriptor {};

		// The drawing pipeline requirements
		PipelineGraphics _textureDisplayPipeline;
		std::shared_ptr<Descriptor> _textureDisplayDescriptor {};
};

