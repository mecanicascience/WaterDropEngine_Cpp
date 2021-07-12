#pragma once

#include "WdeCommon/WdeError/WdeStatus.hpp"
#include "WdeCommon/WdeLogger/Logger.hpp"
#include "WdeCommon/WdeError/WdeException.hpp"
#include "WdeCore/WdeInstance.hpp"

namespace wde {
	class WaterDropEngine {
		public:
			WaterDropEngine() = default;
			~WaterDropEngine() = default;

			/**
			 * Setup the rendering engine
			 * @param graphicsPipeline
			 * @param renderer
			 * @param vertices
			 * @param indices
			 */
			void setupRenderEngine(renderEngine::GraphicsPipeline& graphicsPipeline, renderEngine::Renderer& renderer, std::vector<renderEngine::Model::Vertex>& vertices, std::vector<uint16_t>& indices);

			/** Initialize the program */
			WdeStatus initialize();


		private:
			// RenderingEngine
			renderEngine::GraphicsPipeline* graphicsPipeline = nullptr;
			renderEngine::Renderer* renderer = nullptr;
			std::vector<renderEngine::Model::Vertex>* vertices = nullptr;
			std::vector<uint16_t>* indices = nullptr;
	};
}
