#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeRenderingEngine/descriptors/DescriptorSet.hpp"
#include "../../WdeRenderingEngine/descriptors/Descriptor.hpp"
#include "../../WdeRenderingEngine/descriptors/DescriptorPool.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "../../WdeRenderingEngine/renderer/descriptors/RenderStage.hpp"
#include "../../WdeRenderingEngine/commands/CommandBuffer.hpp"
#include "../objects/GameObject.hpp"
#include "../modules/CameraModule.hpp"

namespace wde::scene {
	class Material {
		/**
		 * Material simple constant push data
		 */
		struct PushConstantData {
			alignas(16) glm::vec3 ambientLightVector {0.0f, 1.0f, 0.0f};
		};

		public:
			// Constructors
			/**
			 * Creates a new unique material
			 * @param materialName The unique name of the material
			 * @param stage The stage in which the material can render
			 * @param vertexShader The link to the pipeline vertex shader
			 * @param fragmentShader The link to the pipeline fragment shader
			 * @param polygonMode The pipeline polygon drawing mode
			 */
			Material(std::string materialName, RenderStage stage, const std::string& vertexShader, const std::string& fragmentShader, VkPolygonMode polygonMode)
				: _materialName(std::move(materialName)),
				  _stage(stage),
				  _vertexShaderName(vertexShader), _fragmentShaderName(fragmentShader),
				  _polygonMode(polygonMode),
				  _pipeline(stage, {"res/shaders/" + vertexShader + ".spv", "res/shaders/" + fragmentShader + ".spv"},
							{ scene::Model::Vertex::getDescriptions() },
							PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, polygonMode,
							VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE) {}


			// Core functions
			/** Initialize the material descriptor and pipeline */
			void initialize(std::shared_ptr<Descriptor>& descriptor) {
				WDE_PROFILE_FUNCTION();

				// Setup descriptor
				if (descriptor != nullptr) {
					// Set descriptor
					_descriptor = descriptor;

					// Add material set (binding 2)
					// ...

					// Reference descriptor to pipeline
					_pipeline.setDescriptor(_descriptor);
				}


				// Setup push constants
				_pipeline.addPushConstants(0, sizeof(PushConstantData));

				// Initialize pipeline
				_pipeline.initialize();
			}

			/**
			 * Binds the material pipeline to the command buffer
			 * @param commandBuffer
			 */
			void bind(CommandBuffer& commandBuffer) {
				WDE_PROFILE_FUNCTION();
				// Bind pipeline
				_pipeline.bind(commandBuffer);

				// Bind the descriptor to the pipeline
				if (_descriptor != nullptr)
					_pipeline.bind(_descriptor);
			}

			/**
			 * Push constants to the material pipeline
			 * @param gameObject
			 * @param camera
			 */
			void pushConstants(GameObject& gameObject, GameObject& camera) {
				// Set push constants
				auto& cameraModule = camera.getModule<scene::CameraModule>();
				PushConstantData push {};
				push.ambientLightVector = glm::normalize(glm::vec3(0.7f, 0.0f, -0.1f));
				_pipeline.setPushConstants(0, &push);
			}

			/**
			 * Push descriptors buffers to the material pipeline
			 * @param gameObject
			 * @param camera
			 */
			void pushDescriptors(GameObject& gameObject, GameObject& camera) {
				// Update material descriptor buffers
				// ...
			}


			// Getters and setters
			RenderStage getStage() { return _stage; }
			std::string& getMaterialName() { return _materialName; }
			std::string& getVertexShader() { return _vertexShaderName; }
			std::string& getFragmentShader() { return _fragmentShaderName; }
			VkPolygonMode getPolygonMode() { return _polygonMode; }


		private:
			/** Unique name of the material type */
			std::string _materialName;
			/** The corresponding material graphics render pipeline */
			PipelineGraphics _pipeline;
			/** The stage in which the material will be rendered to */
			RenderStage _stage;

			/** The associated game object descriptor (material is set binding 3) */
			std::shared_ptr<Descriptor> _descriptor {};


			// Pipeline parameters
			std::string _vertexShaderName;
			std::string _fragmentShaderName;
			VkPolygonMode _polygonMode;
	};
}

