#include "CullingInstance.hpp"

#include <utility>
#include "../../WaterDropEngine.hpp"

namespace wde::scene {
	// Core functions
	CullingInstance::CullingInstance(std::pair<int, int> renderStage, const std::unique_ptr<render::Buffer> &sceneObjectsBuffer) : _renderStage(std::move(renderStage)) {
		WDE_PROFILE_FUNCTION();
		// === Create buffers ===
		int MAX_COMMANDS = Config::MAX_SCENE_OBJECTS_COUNT;

		// List of rendered indirect commands created by the compute shader
		{
			_indirectCommandsBuffer = std::make_unique<render::Buffer>(
					MAX_COMMANDS * sizeof(VkDrawIndexedIndirectCommand),
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);

			// GPU Batches
			_gpuRenderBatches = std::make_unique<render::Buffer>(
					MAX_COMMANDS * sizeof(GPURenderBatch),
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

			// GPU Objects buffer batches and IDs
			_gpuObjectsBatches = std::make_unique<render::Buffer>(
					Config::MAX_SCENE_OBJECTS_COUNT * sizeof(GPUObjectBatch),
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

			// List of game objects IDs in the batches (will match to gl_instanceID, filled by the compute shader)
			_gpuObjectsIDs = std::make_unique<render::Buffer>(
					Config::MAX_SCENE_OBJECTS_COUNT * sizeof(uint32_t),
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

			// GPU buffer that holds the scene data to describe to the compute shader
			_gpuSceneData = std::make_unique<render::Buffer>(
					sizeof(GPUSceneData),
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		}


		// === Create culling pipeline ===
		{
			// Create compute shader scene descriptor set
			render::DescriptorBuilder::begin()
						.bind_buffer(0, *_gpuSceneData, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
						.bind_buffer(1, *sceneObjectsBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
					.build(_generalComputeSet.first, _generalComputeSet.second);

			// Create compute shader resources descriptor set
			render::DescriptorBuilder::begin()
						.bind_buffer(0, *_gpuObjectsBatches, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
						.bind_buffer(1, *_gpuRenderBatches, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
						.bind_buffer(2, *_gpuObjectsIDs, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
						.bind_buffer(3, *_indirectCommandsBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
					.build(_computeSet.first, _computeSet.second);

			// Create compute pipeline
			_cullingPipeline = std::make_unique<render::PipelineCompute>("res/shaders/common/culling/culling_indirect.comp");
			_cullingPipeline->addDescriptorSet(_generalComputeSet.second);
			_cullingPipeline->addDescriptorSet(_computeSet.second);
			_cullingPipeline->initialize();


			// Create drawing descriptor set
			render::DescriptorBuilder::begin()
						.bind_buffer(0, WaterDropEngine::get().getInstance().getPipeline().getCameraBuffer(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
						.bind_buffer(1, *sceneObjectsBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.build(_drawingDescriptorSet.first, _drawingDescriptorSet.second);
		}
	}

	void CullingInstance::createBatches(const std::vector<std::shared_ptr<GameObject>> &gameObjects) {
		WDE_PROFILE_FUNCTION();

		// Clear previous batches
		_renderBatches.clear();

		// Create batches data
		CPURenderBatch currentBatch {};
		std::shared_ptr<scene::Mesh> lastGOMeshRef = nullptr;
		std::shared_ptr<scene::Material> lastGOMaterialRef = nullptr;

		// Render GPU Batches
		void *gpuBatchesData = _gpuRenderBatches->map();
		auto* gpuBatches = (GPURenderBatch*) gpuBatchesData;
		// ------

		// Render GPU Objects batches
		void *gpuObjectsBatchesData = _gpuObjectsBatches->map();
		auto* gpuObjectsBatches = (GPUObjectBatch*) gpuObjectsBatchesData;
		// ------

		// Fetch every game objects
		int goActiveID = 0;
		for (const auto& go : gameObjects) {
			auto meshModule = go->getModule<scene::MeshRendererModule>();

			// If no renderer, or material, or mesh, or if render stage different from culling stage, discard object, push last batch
			if (!go->active || meshModule == nullptr || meshModule->getMaterial() == nullptr || meshModule->getMesh() == nullptr || meshModule->getMaterial()->getRenderStage() != _renderStage) {
				if (currentBatch.indexCount > 0) {
					_renderBatches.push_back(currentBatch);
					// Set gpu batch
					gpuBatches[_renderBatches.size()-1].indexCount = _renderBatches[_renderBatches.size()-1].indexCount;
					gpuBatches[_renderBatches.size()-1].firstIndex = _renderBatches[_renderBatches.size()-1].firstIndex;
					gpuBatches[_renderBatches.size()-1].instanceCount = _renderBatches[_renderBatches.size()-1].instanceCount;
				}

				// No mesh and material
				lastGOMeshRef = nullptr;
				lastGOMaterialRef = nullptr;

				// Empty batch (do not draw this object)
				currentBatch = CPURenderBatch {};
				continue;
			}

			// If material different from last one, push last batch
			auto& mat = meshModule->getMaterial();
			if (currentBatch.indexCount > 0 && lastGOMaterialRef != mat) {
				if (currentBatch.indexCount > 0) {
					_renderBatches.push_back(currentBatch);
					// Set gpu batch
					gpuBatches[_renderBatches.size()-1].indexCount = _renderBatches[_renderBatches.size()-1].indexCount;
					gpuBatches[_renderBatches.size()-1].firstIndex = _renderBatches[_renderBatches.size()-1].firstIndex;
					gpuBatches[_renderBatches.size()-1].instanceCount = _renderBatches[_renderBatches.size()-1].instanceCount;
				}

				// Add this object to a new batch
				currentBatch = CPURenderBatch {};
				currentBatch.material = mat.get();
				currentBatch.mesh = meshModule->getMesh().get();
				currentBatch.firstIndex = goActiveID;
				currentBatch.indexCount = 1;
				currentBatch.instanceCount = 0;

				// Set this object batch
				gpuObjectsBatches[goActiveID].batchID = _renderBatches.size();
				gpuObjectsBatches[goActiveID].objectID = goActiveID;
				gpuObjectsBatches[goActiveID].objectSceneIndex = go->getID();
				gpuObjectsBatches[goActiveID].indicesCount = meshModule->getMesh()->getIndexCount();
				goActiveID++;
				continue;
			}
			lastGOMaterialRef = mat;

			// If mesh different from last one, push last batch
			auto& mesh = meshModule->getMesh();
			if (currentBatch.indexCount > 0 && lastGOMeshRef != mesh) {
				if (currentBatch.indexCount > 0) {
					_renderBatches.push_back(currentBatch);
					// Set gpu batch
					gpuBatches[_renderBatches.size()-1].indexCount = _renderBatches[_renderBatches.size()-1].indexCount;
					gpuBatches[_renderBatches.size()-1].firstIndex = _renderBatches[_renderBatches.size()-1].firstIndex;
					gpuBatches[_renderBatches.size()-1].instanceCount = _renderBatches[_renderBatches.size()-1].instanceCount;
				}

				// Add this object to a new batch
				currentBatch = CPURenderBatch {};
				currentBatch.material = mat.get();
				currentBatch.mesh = mesh.get();
				currentBatch.firstIndex = goActiveID;
				currentBatch.indexCount = 1;
				currentBatch.instanceCount = 0;

				// Set this object batch
				gpuObjectsBatches[goActiveID].batchID = _renderBatches.size();
				gpuObjectsBatches[goActiveID].objectID = goActiveID;
				gpuObjectsBatches[goActiveID].objectSceneIndex = go->getID();
				goActiveID++;
				continue;
			}
			lastGOMeshRef = mesh;

			// Same material and mesh
			currentBatch.material = mat.get();
			currentBatch.mesh = mesh.get();
			currentBatch.indexCount++;
			if (currentBatch.firstIndex == 0 && goActiveID != 0)
				currentBatch.firstIndex = goActiveID;

			// Set this object batch
			gpuObjectsBatches[goActiveID].batchID = _renderBatches.size();
			gpuObjectsBatches[goActiveID].objectID = goActiveID;
			gpuObjectsBatches[goActiveID].objectSceneIndex = go->getID();
			gpuObjectsBatches[goActiveID].indicesCount = meshModule->getMesh()->getIndexCount();

			goActiveID++;
		}

		// Push last batch
		if (currentBatch.indexCount > 0) {
			_renderBatches.push_back(currentBatch);
			// Set gpu batch
			gpuBatches[_renderBatches.size()-1].indexCount = _renderBatches[_renderBatches.size()-1].indexCount;
			gpuBatches[_renderBatches.size()-1].firstIndex = _renderBatches[_renderBatches.size()-1].firstIndex;
			gpuBatches[_renderBatches.size()-1].instanceCount = _renderBatches[_renderBatches.size()-1].instanceCount;
		}

		// Set objects count
		_renderBatchesObjectCount = goActiveID;

		// Unmap buffers
		_gpuObjectsBatches->unmap();
		_gpuRenderBatches->unmap();
	}

	void CullingInstance::cull(const std::shared_ptr<GameObject> &cullingCamera) {
		WDE_PROFILE_FUNCTION();
		// Update scene
		updateScene(cullingCamera);

		// Do culling
		render::CommandBuffer cullingCmd {true};

		// Clear go ids buffer
		vkCmdFillBuffer(cullingCmd, _gpuObjectsIDs->getBuffer(), 0, VK_WHOLE_SIZE, -1);

		// Bind pipeline and descriptors
		_cullingPipeline->bind(cullingCmd);
		vkCmdBindDescriptorSets(cullingCmd, VK_PIPELINE_BIND_POINT_COMPUTE,
		                        _cullingPipeline->getLayout(), 0, 1, &_generalComputeSet.first, 0, nullptr);
		vkCmdBindDescriptorSets(cullingCmd, VK_PIPELINE_BIND_POINT_COMPUTE,
		                        _cullingPipeline->getLayout(), 1, 1, &_computeSet.first, 0, nullptr);

		// Run compute shader
		vkCmdDispatch(cullingCmd, std::ceil(((float) _renderBatchesObjectCount) / 256.0), 1, 1);
		cullingCmd.end();
		cullingCmd.submit();
		cullingCmd.waitForQueueIdle();
	}

	void CullingInstance::render(render::CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		// Read GPU Batches
		void *gpuBatchesData = _gpuRenderBatches->map();
		auto* gpuBatches = (GPURenderBatch*) gpuBatchesData;
		// ------

		// Render batches
		scene::Material* lastMaterial = nullptr;
		int goActiveID = 0;
		for (auto& batch : _renderBatches) {
			// If batch entirely culled, continue
			if (gpuBatches[goActiveID].instanceCount == 0) {
				goActiveID++;
				continue;
			}

			// Different material binding
			if (lastMaterial == nullptr || lastMaterial->getID() != batch.material->getID()) {
				lastMaterial = batch.material;
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				                        batch.material->getPipeline().getLayout(), 0, 1, &_drawingDescriptorSet.first, 0, nullptr);
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
		_gpuRenderBatches->unmap();
		// ------
	}



	// Helper functions
	void CullingInstance::updateScene(const std::shared_ptr<GameObject> &cullingCamera) {
		WDE_PROFILE_FUNCTION();
		if (cullingCamera == nullptr)
			throw WdeException(LogChannel::SCENE, "Trying to run culling without a camera set.");

		GPUSceneData sceneData {};

		// Camera data
		auto cameraModule = cullingCamera->getModule<scene::CameraModule>();
		sceneData.view = cameraModule->getView();

		glm::mat4 projection = cameraModule->getProjection();
		glm::mat4 projectionT = glm::transpose(projection);
		glm::vec4 frustumX = normalizePlane(projectionT[3] + projectionT[0]); // x + w < 0
		glm::vec4 frustumY = normalizePlane(projectionT[3] + projectionT[1]); // y + w < 0

		sceneData.frustum.x = frustumX.x; // left
		sceneData.frustum.y = frustumX.z; // right
		sceneData.frustum.z = frustumY.y; // top
		sceneData.frustum.w = frustumY.z; // bottom

		sceneData.zPlanes.x = cameraModule->getNear();
		sceneData.zPlanes.y = cameraModule->getFar();

		// Update objects count
		sceneData.objectsCount = _renderBatchesObjectCount;

		// Map data
		void *data = _gpuSceneData->map();
		memcpy(data, &sceneData, sizeof(GPUSceneData));
		_gpuSceneData->unmap();
	}
}
