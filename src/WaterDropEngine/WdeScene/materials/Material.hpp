#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "../../WdeRenderingEngine/renderer/descriptors/RenderStage.hpp"
#include "../../WdeRenderingEngine/commands/CommandBuffer.hpp"
#include "../objects/GameObject.hpp"

namespace wde::scene {
	class Material {
		/**
		 * Matrix used in the shaders to perform projection onto the culling Vulkan space
		 */
		struct PushConstantData {
			glm::mat4 transformWorldSpace {1.0f};
			glm::mat4 transformCameraSpace {1.0f};
			glm::mat4 transformProjSpace {1.0f};
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
			Material(std::string  materialName, RenderStage stage, const std::string& vertexShader, const std::string& fragmentShader, VkPolygonMode polygonMode)
				: _materialName(std::move(materialName)),
				  _stage(stage),
				  _vertexShaderName(vertexShader), _fragmentShaderName(fragmentShader),
				  _polygonMode(polygonMode),
				  _pipeline(stage, {"res/shaders/" + vertexShader + ".spv", "res/shaders/" + fragmentShader + ".spv"},
							{ scene::Model::Vertex::getDescriptions() },
							PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, polygonMode,
							VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE) {
				WDE_PROFILE_FUNCTION();
				// Setup push constants and initialize pipeline
				_pipeline.addPushConstants(0, sizeof(PushConstantData));
				_pipeline.initialize();
			}


			// Core functions
			/**
			 * Binds the material to the gameObject and to the command buffer
			 * @param gameObject
			 * @param commandBuffer
			 * @param camera
			 */
			void bind(GameObject& gameObject, CommandBuffer& commandBuffer, GameObject& camera) {
				WDE_PROFILE_FUNCTION();
				// Bind pipeline
				_pipeline.bind(commandBuffer);

				// Set push constants
				auto& cameraModule = camera.getModule<scene::CameraModule>();
				PushConstantData push {};
				push.transformWorldSpace  = gameObject.getModule<TransformModule>().getTransform();
				push.transformCameraSpace = cameraModule.getView();
				push.transformProjSpace   = cameraModule.getProjection();
				_pipeline.setPushConstants(0, &push);
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


			// Pipeline parameters
			std::string _vertexShaderName;
			std::string _fragmentShaderName;
			VkPolygonMode _polygonMode;
	};
}

