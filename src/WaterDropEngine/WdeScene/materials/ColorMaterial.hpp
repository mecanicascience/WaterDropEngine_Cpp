#pragma once

namespace wde::scene {
	class ColorMaterial : public Material {
		public:
			explicit ColorMaterial(RenderStage stage)
				: Material("Color Material", stage, "colorShader.vert", "colorShader.frag", VK_POLYGON_MODE_FILL) {}
	};
}

