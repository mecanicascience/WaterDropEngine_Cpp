#pragma once

#include <WaterDropEngine/includes.hpp>

class NormalMaterial : public Material {
	public:
		explicit NormalMaterial(RenderStage stage)
			: Material("Normal Material", stage, "WaterApp/normalShader.vert", "WaterApp/normalShader.frag") {}
};
