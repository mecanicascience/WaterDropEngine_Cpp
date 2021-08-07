#pragma once

#include "Module.hpp"

namespace wde::scene {
	class ModelModule : public Module {
		public:
			/**
			 * Add a mesh to a game object
			 * @param model
			 */
			explicit ModelModule(GameObject &gameObject, std::shared_ptr<Model> model) :  _model(std::move(model)), Module(gameObject, "Model") { }


			void render(CommandBuffer& commandBuffer) override {
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
	};
}

