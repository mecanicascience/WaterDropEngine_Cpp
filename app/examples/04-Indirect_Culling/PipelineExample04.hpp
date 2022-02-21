#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"
#include "../../../src/WaterDropEngine/WdeRender/pipelines/PipelineCompute.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample04 : public WdeRenderPipelineInstance {
		public:
			/** Stores a rendering batch on the CPU */
			struct RenderBatch {
				scene::Material* material {nullptr}; // Material of the batch
				scene::Mesh* mesh {nullptr}; // Mesh of the batch
				int firstIndex {-1}; // Index of the first object in the batch
				int indexCount {0}; // Number of objects in the batch (batch goes from firstIndex to firstIndex + indexCount)
				int instanceCount {0}; // Number of objects to be drawn after culling (batch goes from firstIndex to firstIndex + instanceCount)
			};
			/** Stores a rendering batch on the GPU */
			struct GPURenderBatch {
				int firstIndex {-1}; // Index of the first object in the batch
				int indexCount {0}; // Number of objects in the batch (batch goes from firstIndex to firstIndex + indexCount)
				int instanceCount {0}; // Number of objects to be drawn after culling (batch goes from firstIndex to firstIndex + instanceCount)
			};
			/** Stores the object batches on the GPU */
			struct GPUObjectBatch {
				int objectID;
				int batchID;
				int objectSceneIndex;
			};
			/** Describes the compute scene */
			struct GPUPushConstantCullingData {
				int objectsCount;
			};


			std::unique_ptr<Buffer> _indirectCommandsBuffer {};
			std::unique_ptr<Buffer> _objectsBatches {};
			std::unique_ptr<Buffer> _gpuBatches {};
			std::unique_ptr<Buffer> _gpuObjectIDs {};
			std::unique_ptr<Buffer> _cullingCameraData {};

			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _generalComputeSet;
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _computeSet;
			std::unique_ptr<PipelineCompute> _cullingPipeline;

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

				// Create buffers
				{
					// Indirect drawing buffer
					int MAX_COMMANDS = Config::MAX_SCENE_OBJECTS_COUNT;
					_indirectCommandsBuffer = std::make_unique<Buffer>(
							MAX_COMMANDS * sizeof(VkDrawIndexedIndirectCommand),
							VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);

					// GPU Objects buffer batches and IDs
					_objectsBatches = std::make_unique<Buffer>(
							Config::MAX_SCENE_OBJECTS_COUNT * sizeof(GPUObjectBatch),
							VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

					// GPU Batches
					_gpuBatches = std::make_unique<Buffer>(
							MAX_COMMANDS * sizeof(GPURenderBatch),
							VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

					// Objects ID (will match to gl_instanceID)
					_gpuObjectIDs = std::make_unique<Buffer>(
							Config::MAX_SCENE_OBJECTS_COUNT * sizeof(int),
							VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

					// Buffer that holds the data of the culling camera
					_cullingCameraData = std::make_unique<Buffer>(
							sizeof(GPUCameraData),
							VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
				}

				// Create culling pipeline
				{
					// Create compute shader scene descriptor set
					DescriptorBuilder::begin()
							.bind_buffer(0, &_cullingCameraData->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
							.bind_buffer(1, &_objectsData->getBufferInfo(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
							.build(_generalComputeSet.first, _generalComputeSet.second);

					// Create compute shader resources descriptor set
					DescriptorBuilder::begin()
							.bind_buffer(0, &_objectsBatches->getBufferInfo(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
							.bind_buffer(1, &_gpuBatches->getBufferInfo(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
							.bind_buffer(2, &_gpuObjectIDs->getBufferInfo(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
							.build(_computeSet.first, _computeSet.second);

					// Create compute pipeline
					_cullingPipeline = std::make_unique<PipelineCompute>("res/shaders/common/culling/culling_indirect.comp.spv");
					_cullingPipeline->addPushConstants(sizeof(GPUPushConstantCullingData));
					_cullingPipeline->addDescriptorSet(_generalComputeSet.second);
					_cullingPipeline->addDescriptorSet(_computeSet.second);
					_cullingPipeline->initialize();
				}
			}

			void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override {
				// Create rendering batches
				std::vector<RenderBatch> renderBatches {};
				int objectsCount;
				{
					WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::createRenderBatches");
					RenderBatch currentBatch {};

					std::shared_ptr<scene::Mesh> lastGOMeshRef = nullptr;
					std::shared_ptr<scene::Material> lastGOMaterialRef = nullptr;

					// Render GPU Objects batches
					void *gpuObjectsBatchesData = _objectsBatches->map();
					auto* gpuObjectsBatches = (GPUObjectBatch*) gpuObjectsBatchesData;
					// ------

					// Render GPU Batches
					void *gpuBatchesData = _gpuBatches->map();
					auto* gpuBatches = (GPURenderBatch*) gpuBatchesData;
					// ------

					// Fetch every game objects
					int goActiveID = 0;
					for (auto& go : scene.getGameObjects()) {
						auto meshModule = go->getModule<scene::MeshRendererModule>();

						// If no renderer, or material, or mesh, push last batch
						if (meshModule == nullptr || meshModule->getMaterial() == nullptr || meshModule->getMesh() == nullptr) {
							if (currentBatch.indexCount > 0) {
								renderBatches.push_back(currentBatch);
								// Set gpu batch
								gpuBatches[renderBatches.size()-1].indexCount = renderBatches[renderBatches.size()-1].indexCount;
								gpuBatches[renderBatches.size()-1].firstIndex = renderBatches[renderBatches.size()-1].firstIndex;
								gpuBatches[renderBatches.size()-1].instanceCount = renderBatches[renderBatches.size()-1].instanceCount;
							}

							// No mesh and material
							lastGOMeshRef = nullptr;
							lastGOMaterialRef = nullptr;

							// Empty batch (do not draw this object)
							currentBatch = RenderBatch {};
							continue;
						}

						// If material different from last one, push last batch
						auto& mat = meshModule->getMaterial();
						if (currentBatch.indexCount > 0 && lastGOMaterialRef != mat) {
							if (currentBatch.indexCount > 0) {
								renderBatches.push_back(currentBatch);
								// Set gpu batch
								gpuBatches[renderBatches.size()-1].indexCount = renderBatches[renderBatches.size()-1].indexCount;
								gpuBatches[renderBatches.size()-1].firstIndex = renderBatches[renderBatches.size()-1].firstIndex;
								gpuBatches[renderBatches.size()-1].instanceCount = renderBatches[renderBatches.size()-1].instanceCount;
							}

							// Add this object to a new batch
							currentBatch = RenderBatch {};
							currentBatch.material = mat.get();
							currentBatch.mesh = meshModule->getMesh().get();
							currentBatch.firstIndex = static_cast<int>(goActiveID);
							currentBatch.indexCount = 1;
							currentBatch.instanceCount = 0;

							// Set this object batch
							gpuObjectsBatches[goActiveID].batchID = static_cast<int>(renderBatches.size());
							gpuObjectsBatches[goActiveID].objectID = goActiveID;
							gpuObjectsBatches[goActiveID].objectSceneIndex = static_cast<int>(go->getID());
							goActiveID++;
							continue;
						}
						lastGOMaterialRef = mat;

						// If mesh different from last one, push last batch
						auto& mesh = meshModule->getMesh();
						if (currentBatch.indexCount > 0 && lastGOMeshRef != mesh) {
							if (currentBatch.indexCount > 0) {
								renderBatches.push_back(currentBatch);
								// Set gpu batch
								gpuBatches[renderBatches.size()-1].indexCount = renderBatches[renderBatches.size()-1].indexCount;
								gpuBatches[renderBatches.size()-1].firstIndex = renderBatches[renderBatches.size()-1].firstIndex;
								gpuBatches[renderBatches.size()-1].instanceCount = renderBatches[renderBatches.size()-1].instanceCount;
							}

							// Add this object to a new batch
							currentBatch = RenderBatch {};
							currentBatch.material = mat.get();
							currentBatch.mesh = mesh.get();
							currentBatch.firstIndex = static_cast<int>(goActiveID);
							currentBatch.indexCount = 1;
							currentBatch.instanceCount = 0;

							// Set this object batch
							gpuObjectsBatches[goActiveID].batchID = static_cast<int>(renderBatches.size());
							gpuObjectsBatches[goActiveID].objectID = goActiveID;
							gpuObjectsBatches[goActiveID].objectSceneIndex = static_cast<int>(go->getID());
							goActiveID++;
							continue;
						}
						lastGOMeshRef = mesh;

						// Same material and mesh
						currentBatch.material = mat.get();
						currentBatch.mesh = mesh.get();
						currentBatch.indexCount++;
						if (currentBatch.firstIndex == -1)
							currentBatch.firstIndex = static_cast<int>(goActiveID);

						// Set this object batch
						gpuObjectsBatches[goActiveID].batchID = static_cast<int>(renderBatches.size());
						gpuObjectsBatches[goActiveID].objectID = goActiveID;
						gpuObjectsBatches[goActiveID].objectSceneIndex = static_cast<int>(go->getID());

						goActiveID++;
					}

					// Push last batch
					if (currentBatch.indexCount > 0) {
						renderBatches.push_back(currentBatch);
						// Set gpu batch
						gpuBatches[renderBatches.size()-1].indexCount = renderBatches[renderBatches.size()-1].indexCount;
						gpuBatches[renderBatches.size()-1].firstIndex = renderBatches[renderBatches.size()-1].firstIndex;
						gpuBatches[renderBatches.size()-1].instanceCount = renderBatches[renderBatches.size()-1].instanceCount;
					}
					objectsCount = goActiveID;

					// Close render gpu batches
					_gpuBatches->unmap();
					// ------

					// Close objects gpu batches
					_objectsBatches->unmap();
					// ------
				}


				// Set culling camera
				auto& cameraGO = scene.getGameObject(1); // scene.getActiveCamera();
				{
					auto cameraModule = cameraGO.getModule<scene::CameraModule>();

					GPUCameraData cameraData {};
					cameraData.proj = cameraModule->getProjection();
					cameraData.view = cameraModule->getView();

					// Map data
					void *data = _cullingCameraData->map();
					memcpy(data, &cameraData, sizeof(GPUCameraData));
					_cullingCameraData->unmap();
				}


				// Run culling
				{
					CommandBuffer cullingCmd {true};

					// Clear go id buffer
					vkCmdFillBuffer(cullingCmd, _gpuObjectIDs->getBuffer(), 0, VK_WHOLE_SIZE, -1);

					// Bind pipeline
					_cullingPipeline->bind(cullingCmd);

					// Bind general descriptor set at binding 0
					vkCmdBindDescriptorSets(cullingCmd, VK_PIPELINE_BIND_POINT_COMPUTE,
					                        _cullingPipeline->getLayout(), 0, 1, &_generalComputeSet.first, 0, nullptr);

					// Bind objects descriptor set at binding 1
					vkCmdBindDescriptorSets(cullingCmd, VK_PIPELINE_BIND_POINT_COMPUTE,
					                        _cullingPipeline->getLayout(), 1, 1, &_computeSet.first, 0, nullptr);

					// Update push constants
					GPUPushConstantCullingData gpuCulling {};
					gpuCulling.objectsCount = objectsCount;
					_cullingPipeline->setPushConstants(&gpuCulling);

					// Run compute shader
					vkCmdDispatch(cullingCmd, std::ceil(((float) objectsCount) / 256.0), 1, 1);
					cullingCmd.end();
					cullingCmd.submit();
					cullingCmd.waitForQueueIdle();
				}


				// Create render commands
				{
					WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::recordRenderCommands");
					// Read GPU GO Batches
					void *gpuBatchesGOData = _gpuObjectIDs->map();
					auto* gpuBatchesGO = (int*) gpuBatchesGOData;
					// ------

					// Map Render GO Commands
					void *renderGOData = _indirectCommandsBuffer->map();
					auto* commandsGOData = (VkDrawIndexedIndirectCommand*) renderGOData;
					// ------

					// Create game object render command
					int goActiveID = 0;
					for (const auto& go : scene.getGameObjects()) {
						auto meshMod = go->getModule<scene::MeshRendererModule>();
						if (meshMod == nullptr || meshMod->getMesh() == nullptr || meshMod->getMaterial() == nullptr) // Culled object
							continue;
						if (gpuBatchesGO[goActiveID] != -1)
							commandsGOData[goActiveID] = meshMod->getMesh()->getRenderIndirectCommand(gpuBatchesGO[goActiveID]);
						goActiveID++;
					}

					// Unmap Render GO Commands
					_indirectCommandsBuffer->unmap();
					// ------

					// Unmap GPU GO Batches
					_gpuObjectIDs->unmap();
					// ------
				}


				beginRenderPass(0);
					beginRenderSubPass(0);
						// Read GPU Batches
						void *gpuBatchesData = _gpuBatches->map();
						auto* gpuBatches = (GPURenderBatch*) gpuBatchesData;
						// ------

						// Render batches
						scene::Material* lastMaterial = nullptr;
						int goActiveID = 0;
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
							VkDeviceSize indirectOffset = gpuBatches[goActiveID].firstIndex * sizeof(VkDrawIndexedIndirectCommand);
							uint32_t drawStride = sizeof(VkDrawIndexedIndirectCommand);
							vkCmdDrawIndexedIndirect(commandBuffer, _indirectCommandsBuffer->getBuffer(), indirectOffset, gpuBatches[goActiveID].instanceCount, drawStride);

							// Iterate through game objects
							goActiveID++;
						}

						// Unmap GPU Batches
						_gpuBatches->unmap();
						// ------
					endRenderSubPass();

					beginRenderSubPass(1);
						// Render GUI
						gui::WdeGUI::render(commandBuffer);
					endRenderSubPass();
				endRenderPass();
			}

			void cleanUp() override { }
	};
}
