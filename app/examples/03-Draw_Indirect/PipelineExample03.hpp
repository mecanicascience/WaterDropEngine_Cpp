#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample03 : public WdeRenderPipelineInstance {
		public:
			/**
			 * Stores a rendering batch
			 */
			struct RenderBatch {
				resource::Material* material {nullptr}; // Material of the batch
				resource::Mesh* mesh {nullptr}; // Mesh of the batch
				int firstIndex {-1}; // Index of the first object in the batch
				int indexCount {0}; // Number of objects in the batch (batch goes from firstIndex to firstIndex + indexCount)
			};
			std::unique_ptr<Buffer> _indirectCommandsBuffer {};

			void setup() override {
				// Create passes attachments
				setAttachments({
					{0, "Depth texture", RenderAttachment::DEPTH},
					{1, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
                });

				// Create passes and subpasses structure
				setStructure({
					{0, {
						{0, {}, { 0, 1 }},
						{1, {}, { 1 }}
					}}
				});

				// Initialize GUI
				gui::WdeGUI::initialize(std::pair<int, int>{0, 1});

				// Indirect drawing buffer
				int MAX_COMMANDS = Config::MAX_CHUNK_OBJECTS_COUNT;
				_indirectCommandsBuffer = std::make_unique<Buffer>(
						MAX_COMMANDS * sizeof(VkDrawIndexedIndirectCommand),
						VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
			}

			void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override {
				/*// Create rendering batches
				std::vector<RenderBatch> renderBatches {};
				{
					WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::createRenderBatches");
					RenderBatch currentBatch {};

					resource::Mesh* lastGOMeshRef = nullptr;
					resource::Material* lastGOMaterialRef = nullptr;

					// Map Render GO Commands
					void *renderGOData = _indirectCommandsBuffer->map();
					auto* commandsGOData = (VkDrawIndexedIndirectCommand*) renderGOData;
					// ------

					// Fetch every game objects
					int goActiveID = 0;
					for (auto& go : scene.getGameObjects()) {
						auto meshModule = go->getModule<scene::MeshRendererModule>();

						// If no renderer, or material, or mesh, push last batch
						if (!go->active || meshModule == nullptr || meshModule->getMaterial() == nullptr || meshModule->getMesh() == nullptr) {
							if (currentBatch.indexCount > 0)
								renderBatches.push_back(currentBatch);

							// No mesh and material
							lastGOMeshRef = nullptr;
							lastGOMaterialRef = nullptr;

							// Empty batch (do not draw this object)
							currentBatch = RenderBatch {};
							continue;
						}

						// Create game object render command
						commandsGOData[goActiveID] = meshModule->getMesh()->getRenderIndirectCommand(go->getID());

						// If material different from last one, push last batch
						auto mat = meshModule->getMaterial();
						if (currentBatch.indexCount > 0 && lastGOMaterialRef != mat) {
							if (currentBatch.indexCount > 0)
								renderBatches.push_back(currentBatch);

							// Add this object to a new batch
							currentBatch = RenderBatch {};
							currentBatch.material = mat;
							currentBatch.mesh = meshModule->getMesh();
							currentBatch.firstIndex = static_cast<int>(goActiveID);
							currentBatch.indexCount = 1;
							goActiveID++;
							continue;
						}
						lastGOMaterialRef = mat;

						// If mesh different from last one, push last batch
						auto mesh = meshModule->getMesh();
						if (currentBatch.indexCount > 0 && lastGOMeshRef != mesh) {
							if (currentBatch.indexCount > 0)
								renderBatches.push_back(currentBatch);

							// Add this object to a new batch
							currentBatch = RenderBatch {};
							currentBatch.material = mat;
							currentBatch.mesh = mesh;
							currentBatch.firstIndex = static_cast<int>(goActiveID);
							currentBatch.indexCount = 1;
							goActiveID++;
							continue;
						}
						lastGOMeshRef = mesh;

						// Same material and mesh
						currentBatch.material = mat;
						currentBatch.mesh = mesh;
						currentBatch.indexCount++;
						if (currentBatch.firstIndex == -1)
							currentBatch.firstIndex = static_cast<int>(goActiveID);
						goActiveID++;
					}

					// Push last batch
					if (currentBatch.indexCount > 0)
						renderBatches.push_back(currentBatch);

					// Unmap Render GO Commands
					_indirectCommandsBuffer->unmap();
					// ------
				}


				beginRenderPass(0);
					beginRenderSubPass(0);
						// Render batches
						resource::Material* lastMaterial = nullptr;
						for (auto& batch : renderBatches) {
							// Different material binding
							if (lastMaterial == nullptr || lastMaterial->getID() != batch.material->getID()) {
								lastMaterial = batch.material;
								bind(commandBuffer, batch.material);
								batch.material->bind(commandBuffer);
							}

							// Mesh binding
							batch.mesh->bind(commandBuffer);

							// Execute the draw command buffer on each section as defined by the array of draws
							VkDeviceSize indirectOffset = batch.firstIndex * sizeof(VkDrawIndexedIndirectCommand);
							uint32_t drawStride = sizeof(VkDrawIndexedIndirectCommand);
							vkCmdDrawIndexedIndirect(commandBuffer, _indirectCommandsBuffer->getBuffer(), indirectOffset, batch.indexCount, drawStride);
						}
					endRenderSubPass();

					beginRenderSubPass(1);
						// Render GUI
						gui::WdeGUI::render(commandBuffer);
					endRenderSubPass();
				endRenderPass();*/
			}

			void cleanUp() override { }
	};
}
