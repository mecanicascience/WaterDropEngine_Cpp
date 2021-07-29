#pragma once

#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/Subrenderer.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/structures/Model.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/structures/GameObject.hpp"

namespace wde::renderEngine {
	class CoreAppSubrenderer : public Subrenderer {
		public:
			/** Creates the renderer given the stage */
			explicit CoreAppSubrenderer(const RenderStage &stage);

			/** Records commands on the given command buffer */
			void render(CommandBuffer &commandBuffer) override;

			// Add a game object to the list
			void addGameObject(GameObject& gameObject) { _gameObjects.push_back(gameObject); }

		private:
			/** The rendering graphics pipeline */
			PipelineGraphics _pipeline;
			/** The rendered game objects */
			std::vector<GameObject> _gameObjects;
	};
}
