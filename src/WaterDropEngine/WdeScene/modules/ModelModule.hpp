#pragma once

#include "Module.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"

namespace wde::scene {
	class ModelModule : public Module {
		/** Matrix used in the shaders to perform projection onto the culling Vulkan space */
		struct PushConstantData {
			glm::mat4 transformWorldSpace {1.0f};
			glm::mat4 transformCameraSpace {1.0f};
			glm::mat4 transformProjSpace {1.0f};
		};

		public:
			/**
			 * Add a mesh to a game object
			 * @param gameObject A reference to the parent game object (auto set by the Scene)
			 * @param model The game object model
			 * @param stage The render stage of the model
			 */
			explicit ModelModule(GameObject &gameObject, std::shared_ptr<Model> model, const RenderStage &stage)
				:  _model(std::move(model)),
				   _pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"}, { scene::Model::Vertex::getDescriptions() },
							 PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
							 VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE),
				   _stage(stage),
				   Module(gameObject, "Model") {
				WDE_PROFILE_FUNCTION();
				// Setup push constants and initialize pipeline
				_pipeline.addPushConstants(0, sizeof(PushConstantData));
				_pipeline.initialize();
			}


			void render(CommandBuffer& commandBuffer, RenderStage stage, GameObject& camera) override {
				WDE_PROFILE_FUNCTION();
				if (stage != _stage) // Not in the right rendering stage
					return;

				// Bind pipeline
				_pipeline.bind(commandBuffer);

				// Set push constants
				auto& cameraModule = camera.getModule<scene::CameraModule>();
				PushConstantData push {};
				push.transformWorldSpace  = _gameObject.getModule<TransformModule>().getTransform();
				push.transformCameraSpace = cameraModule.getView();
				push.transformProjSpace   = cameraModule.getProjection();
				_pipeline.setPushConstants(0, &push);

				// Render model
				_model->bind(commandBuffer);
				_model->render();
			}

			void renderGUI() override {
				// Simple GUI
				ImGui::Text("Using object model.");
				ImGui::Text(("Vertices count : " + std::to_string(_model->getVertices().size()) + ".").c_str());
				ImGui::Text(("Indices count : " + std::to_string(_model->getIndices().size()) + ".").c_str());
			}

			void cleanUp() override {
				// Clean up model
				_model->cleanUp();
			}


		private:
			std::shared_ptr<Model> _model;
			PipelineGraphics _pipeline;
			RenderStage _stage;
	};
}

