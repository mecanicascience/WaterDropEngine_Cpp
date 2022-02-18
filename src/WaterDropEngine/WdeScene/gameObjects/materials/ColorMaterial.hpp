#pragma once

#include "Material.hpp"

namespace wde::scene {
	class ColorMaterial : public Material {
		public:
			explicit ColorMaterial(std::pair<int, int> renderStage)
			    : Material("Color Material", renderStage, {
                            "res/shaders/examples/02-3D_Cube/cube.vert.spv",
                            "res/shaders/examples/02-3D_Cube/cube.frag.spv"
                        }) {}
	};
}

