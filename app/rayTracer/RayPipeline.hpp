#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"
#include "../../../src/WaterDropEngine/WdeRender/pipelines/PipelineCompute.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gizmo/GizmoManager.hpp"
#include "../../../src/WaterDropEngine/WdeScene/culling/CullingInstance.hpp"
#include "gui/CImage.hpp"

using namespace wde;
using namespace wde::render;


class RayPipeline : public WdeRenderPipelineInstance {
	public:
		void setup() override;
		void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override;
		void onNotify(const core::Event& event) override;
		void cleanUp() override;

	private:
		std::shared_ptr<CImage> _image;
		uint32_t _viewportWidth = 0;
		uint32_t _viewportHeight = 0;
};
