#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeRender/descriptors/Descriptor.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample02 : public WdeRenderPipelineInstance {
		public:
			std::unique_ptr<PipelineGraphics> _pipeline {};
			std::shared_ptr<Descriptor> _desc {};

			int OBJECT_COUNT = 1;
			struct GPUGameObjectData {
				glm::mat4 objectMatrix {};
			};

			void setup() override {
				// Create passes attachments
				setAttachments({
					{0, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
				});

				// Create passes and subpasses structure
				setStructure({
					{0, {
						{0, {}, { 0 }},
						{1, {}, { 0 }}
					}}
				});

				// Initialize GUI
				gui::WdeGUI::initialize(std::pair<int, int>{0, 1});


				// Create object descriptor
				_desc = std::make_shared<Descriptor>();
				_desc->addSet(0, {
					{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, static_cast<int>(sizeof(GPUGameObjectData) * OBJECT_COUNT)} // Holds 10 objects
				});
				_desc->initialize();


				// Create pipeline
				_pipeline = std::make_unique<PipelineGraphics>(
						std::pair<int, int>{0, 0},
						std::vector<std::string>{
								"res/shaders/examples/01-Triangle/triangle.vert.spv",
								"res/shaders/examples/01-Triangle/triangle.frag.spv"
						}, // Shaders
						PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
						PipelineGraphics::Depth::None,    // Do not use depth
						VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
						VK_POLYGON_MODE_FILL,   // Fill drawn shapes
						VK_CULL_MODE_NONE);   // Disable pipeline culling
				_pipeline->addDescriptor(_desc);
				_pipeline->initialize();


				// Fill descriptor buffer
				GPUGameObjectData objStruct[OBJECT_COUNT];
				for (int i = 0; i < OBJECT_COUNT; i++) {
					objStruct[i].objectMatrix = glm::mat4(1.0f);
				}
				_desc->getSet(0).updateBuffer(0, &objStruct);
			}

			void render(CommandBuffer& commandBuffer) override {
				beginRenderPass(0);
					beginRenderSubPass(0);
						// Bind pipeline and descriptor
						_pipeline->bind(commandBuffer);
						_desc->bind(commandBuffer, *_pipeline);

						// Draw 3 hard-coded vertices in the shader
						vkCmdDraw(commandBuffer, 3, 1, 0, 0);

						/**
						 * TODO
						 * Get active camera (scene -> activeCameraID)
						 * [TODO => do culling writes GO ids to buffer]
						 * Render GO (for each material, bind pipeline + render each game objects corresponding to that pipeline)
						 */
					endRenderSubPass();

					beginRenderSubPass(1);
						// Render GUI
						gui::WdeGUI::render(commandBuffer);
					endRenderSubPass();
				endRenderPass();
			}


			void cleanUp() override {
				_pipeline.reset();
				_desc.reset();
			}
	};
}
