#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeRenderingEngine/descriptors/DescriptorSet.hpp"
#include "../../WdeRenderingEngine/descriptors/Descriptor.hpp"
#include "../../WdeRenderingEngine/descriptors/DescriptorPool.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "../../WdeRenderingEngine/renderer/descriptors/RenderStage.hpp"
#include "../../WdeRenderingEngine/commands/CommandBuffer.hpp"
#include "../modules/CameraModule.hpp"
#include "../../WdeRenderingEngine/images/Texture2D.hpp"

namespace wde::scene {
	class Material {
		public:
			// Constructors
			/**
			 * Creates a new unique material
			 * @param materialName The unique name of the material
			 * @param stage The stage in which the material can render
			 * @param vertexShader The link to the pipeline vertex shader
			 * @param fragmentShader The link to the pipeline fragment shader
			 * @param polygonMode The pipeline polygon drawing mode (default VK_POLYGON_MODE_FILL)
			 */
			Material(std::string materialName, RenderStage stage, const std::string& vertexShader, const std::string& fragmentShader, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL)
				: _materialName(std::move(materialName)),
				  _stage(stage),
				  _vertexShaderName(vertexShader), _fragmentShaderName(fragmentShader),
				  _polygonMode(polygonMode),
				  _pipeline(stage, {"res/shaders/" + vertexShader + ".spv", "res/shaders/" + fragmentShader + ".spv"},
							{ scene::Model::Vertex::getDescriptions() },
							PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::ReadWrite,
							VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, polygonMode,
							VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE) {}


			// Core functions
			/** Initialize the material descriptor and pipeline */
			virtual void initialize(std::shared_ptr<Descriptor>& descriptor) {
				WDE_PROFILE_FUNCTION();

				// Setup descriptor
				if (descriptor != nullptr) {
					// Set descriptor
					_descriptor = descriptor;

					// Setup the material descriptor
					setupDescriptor(descriptor);

					// Reference descriptor to pipeline
					_pipeline.addDescriptor(_descriptor);
				}

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

			/** Render GUI elements to the screen */
			virtual void renderGUI() {
				// Default GUI rendering
				ImGui::Text(("Vertex shader : " + getVertexShader() + ".").c_str());
				ImGui::Text(("Fragment shader : " + getFragmentShader() + ".").c_str());

				std::string polygonMode;
				if (getPolygonMode() == VK_POLYGON_MODE_POINT)
					polygonMode = "Point mode";
				else if (getPolygonMode() == VK_POLYGON_MODE_LINE)
					polygonMode = "Line mode";
				else
					polygonMode = "Fill mode";
				ImGui::Text(("Polygon mode : " + polygonMode + ".").c_str());
			}

			// Serializers
			virtual json serialize() = 0;
			virtual void deserialize(json data) {};


			// Descriptors and push constants
			/**
			 * Setup the material descriptor
			 * @param descriptor
			 */
			virtual void setupDescriptor(std::shared_ptr<Descriptor>& descriptor) {}

			/** Update push constants */
			virtual void pushConstants() {}

			/** Update the descriptor buffers */
			virtual void pushDescriptors() {}


			// Getters and setters
			RenderStage getStage() { return _stage; }
			std::string& getMaterialName() { return _materialName; }
			std::string& getVertexShader() { return _vertexShaderName; }
			std::string& getFragmentShader() { return _fragmentShaderName; }
			VkPolygonMode getPolygonMode() { return _polygonMode; }


		protected:
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

