#pragma once

#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/Renderer.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderPassAttachment.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderSubpassType.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPass.hpp"
#include "../src/WaterDropEngine/WdeCommon/WdeMaths/Color.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/structures/Model.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/structures/GameObject.hpp"

#include "CoreAppSubrenderer.hpp"

using namespace wde;
using namespace wde::renderEngine;

class CoreAppRenderer : public Renderer {
	public:
		CoreAppRenderer() = default;
		~CoreAppRenderer() override = default;

		/** Initialize the renderer */
		void initialize() override;
		/** Starts the renderer */
		void start() override;
};
