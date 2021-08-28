#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"

class DepthMaterial : public Material {
	public:
		explicit DepthMaterial(RenderStage stage)
			: Material("Depth Compute Material", stage, "depth/depthShader.vert", "depth/depthShader.frag") {}
};
