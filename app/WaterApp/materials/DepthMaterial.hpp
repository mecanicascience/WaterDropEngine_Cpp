#pragma once

#include <WaterDropEngine/includes.hpp>

class DepthMaterial : public Material {
	public:
		explicit DepthMaterial(RenderStage stage)
			: Material("Depth Compute Material", stage, "WaterApp/depthComputeShader.vert", "WaterApp/depthComputeShader.frag") {}
};
