#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"
#include "../../WdeRenderingEngine/pipelines/PipelineGraphics.hpp"

class NormalMaterial : public Material {
	public:
		explicit NormalMaterial(RenderStage stage)
			: Material("Normal Material", stage, "normal/normalShader.vert", "normal/normalShader.frag") {}
};
