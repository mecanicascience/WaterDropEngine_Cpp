#pragma once

#include "Module.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "../materials/Material.hpp"

namespace wde::scene {
	class ModelModule : public Module {
		public:
			/**
			 * Add a mesh to a game object
			 * @param gameObject A reference to the parent game object (auto set by the Scene)
			 * @param model The game object model
			 * @param material The corresponding object material
			 */
			explicit ModelModule(GameObject &gameObject, std::shared_ptr<Model> model, std::shared_ptr<Material> material)
				:  _model(std::move(model)), _material(std::move(material)), Module(gameObject, "Model") {}


			void render(CommandBuffer& commandBuffer, RenderStage stage, GameObject& camera) override {
				WDE_PROFILE_FUNCTION();
				if (stage != _material->getStage()) // Not in the right rendering stage
					return;

				// Bind material to the game object
				_material->bind(_gameObject, commandBuffer, camera);

				// Render model
				_model->bind(commandBuffer);
				_model->render();
			}

			void renderGUI() override {
				// Model infos
				ImGui::Text("Object model data.");
				ImGui::Text(("Vertices count : " + std::to_string(_model->getVertices().size()) + ".").c_str());
				ImGui::Text(("Indices count : " + std::to_string(_model->getIndices().size()) + ".").c_str());

				// Material infos
				ImGui::Text("Material data.");
				ImGui::Text(("Vertex shader : " + _material->getVertexShader() + ".").c_str());
				ImGui::Text(("Fragment shader : " + _material->getFragmentShader() + ".").c_str());

				std::string polygonMode;
				if (_material->getPolygonMode() == VK_POLYGON_MODE_POINT)
					polygonMode = "Point mode";
				else if (_material->getPolygonMode() == VK_POLYGON_MODE_LINE)
					polygonMode = "Line mode";
				else
					polygonMode = "Fill mode";
				ImGui::Text(("Polygon mode : " + polygonMode + ".").c_str());
			}

			void cleanUp() override {
				// Clean up model
				_model->cleanUp();
			}


		private:
			std::shared_ptr<Model> _model;
			std::shared_ptr<Material> _material;
	};
}

