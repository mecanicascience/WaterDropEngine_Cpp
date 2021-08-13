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

			// Core functions
			void initialize() override;
			void render(CommandBuffer& commandBuffer, RenderStage stage) override;
			void renderGUI() override;
			void cleanUp() override;

			// Serializers
			json serialize() override;
			void deserialize(json data) override;


		private:
			std::shared_ptr<Model> _model;
			std::shared_ptr<Material> _material;
	};
}

