#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../gameObjects/meshes/Mesh.hpp"
#include "../gameObjects/modules/MeshRendererModule.hpp"
#include "../../WdeRender/pipelines/PipelineCompute.hpp"
#include "../gameObjects/modules/CameraModule.hpp"

namespace wde::scene {
	/**
	 * Describes a set of culling objects.
	 */
	class CullingInstance : public NonCopyable {
		public:
			// ==== Data describing structures =====
			// Render batches
			/** Stores a rendering batch on the CPU */
			struct CPURenderBatch {
				resource::Material *material;     // Material of the batch
				resource::Mesh *mesh;             // Mesh of the batch
				uint32_t firstIndex ;   // Index of the first object in the batch
				uint32_t indexCount;    // Number of objects in the batch (batch goes from firstIndex to firstIndex + indexCount)
				uint32_t instanceCount; // Number of objects to be drawn after culling (batch goes from firstIndex to firstIndex + instanceCount)
			};

			/** Stores a rendering batch on the GPU */
			struct GPURenderBatch {
				uint32_t firstIndex;    // Index of the first object in the batch
				uint32_t indexCount;    // Number of objects in the batch (batch goes from firstIndex to firstIndex + indexCount)
				uint32_t instanceCount; // Number of objects to be drawn after culling (batch goes from firstIndex to firstIndex + instanceCount)
			};

			/** Describes a scene game object corresponding batch data */
			struct GPUObjectBatch {
				uint32_t objectID;           // Objects ID in the batch
				uint32_t batchID;            // ID of the object batch
				uint32_t objectSceneIndex;   // Object UUID
				uint32_t indicesCount;       // Number of indices in the object mesh (used to create object render commands)
			};

			/** Describes the scene data sent to the compute shader */
			struct GPUSceneData {
				glm::mat4 view;
				glm::vec4 frustum;
				glm::vec2 zPlanes;
				int objectsCount;
			};



			// Constructors and destructors
			/**
			 * Creates a new scene culling instance
			 * @param renderStage The allowed material of the render stage
			 * @param sceneObjectsBuffer The culling instance objects buffer
			 */
			explicit CullingInstance(std::pair<int, int> renderStage, const std::unique_ptr<render::Buffer>& sceneObjectsBuffer);



			// Core functions
			/**
			 * Generate render batches from a set of game objects and stores them to _renderBatches.
			 * This will update the GPU objects batch IDs, the GPU render batches list, and create a CPU render batches list.
			 * @param gameObjects The list of culled game objects
			 * @return The render batches vector
			 */
			void createBatches(const std::vector<std::shared_ptr<GameObject>>& gameObjects);

			/**
			 * Do culling based on it's batches for a specific scene camera
			 * @param cullingCamera
			 */
			void cull(const std::shared_ptr<GameObject>& cullingCamera);

			 /**
			  * Draws the objects in the culled buffers
			  * @param commandBuffer Rendering command buffer
			  */
			 void render(render::CommandBuffer& commandBuffer);



		private:
			// Batches storage
			std::pair<int, int> _renderStage;
			std::vector<CPURenderBatch> _renderBatches {}; // List of last render scene batches
			int _renderBatchesObjectCount = 0; // Number of objects in the last render batch

			// Culling data buffers
			/** List of rendered indirect commands created by the compute shader */
			std::unique_ptr<render::Buffer> _indirectCommandsBuffer {};
			/** For each object in a batch, describes it's corresponding batch data */
			std::unique_ptr<render::Buffer> _gpuObjectsBatches {};
			/** Stores the rendering batches list on the GPU */
			std::unique_ptr<render::Buffer> _gpuRenderBatches {};
			/** List of game objects IDs in the batches (will match to gl_instanceID, filled by the compute shader) */
			std::unique_ptr<render::Buffer> _gpuObjectsIDs {};
			/** Describes the scene data sent to the compute shader */
			std::unique_ptr<render::Buffer> _gpuSceneData {};


			// Culling pipeline and pipeline resources
			std::unique_ptr<render::PipelineCompute> _cullingPipeline;
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _generalComputeSet;
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _computeSet;
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _drawingDescriptorSet;



			// Helper functions
			/** Update the culling scene parameters based on the scene and on it's configured camera */
			void updateScene(const std::shared_ptr<GameObject>& cullingCamera);

			inline static glm::vec4 normalizePlane(glm::vec4 p) {
				return p / glm::length(glm::vec3(p));
			}
	};
}

