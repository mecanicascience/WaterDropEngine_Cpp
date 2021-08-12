#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"

namespace wde::scene {
	class ColorMaterial : public Material {
		public:
			explicit ColorMaterial(RenderStage stage)
				: Material("Color Material", stage, "color/colorShader.vert", "color/colorShader.frag") {}
	};
}

