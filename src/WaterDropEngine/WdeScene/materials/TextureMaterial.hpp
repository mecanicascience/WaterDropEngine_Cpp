#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"

namespace wde::scene {
	class TextureMaterial : public Material {
		public:
			explicit TextureMaterial(RenderStage stage)
				: Material("Texture Material", stage, "texture/textureShader.vert", "texture/textureShader.frag") {}
	};
}
