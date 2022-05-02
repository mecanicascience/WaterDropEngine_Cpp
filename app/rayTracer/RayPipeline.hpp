#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"
#include "../../../src/WaterDropEngine/WdeRender/pipelines/PipelineCompute.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gizmo/GizmoManager.hpp"
#include "../../../src/WaterDropEngine/WdeScene/culling/CullingInstance.hpp"
#include "gui/CImage.hpp"
#include "rays/Ray.hpp"
#include "rays/RayTracer.hpp"
#include "utils/Vector.hpp"

using namespace wde;
using namespace wde::render;


class RayPipeline : public WdeRenderPipelineInstance {
	public:
		void setup() override;
		void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override;
		void onNotify(const core::Event& event) override;
		void cleanUp() override;

		void renderRayTracing();

	private:
		std::shared_ptr<CImage> _image;
		std::unique_ptr<RayTracer> _rayTracer;
		uint32_t* _imageData = nullptr;

		int _viewportWidth = 0;
		int _viewportHeight = 0;
};
