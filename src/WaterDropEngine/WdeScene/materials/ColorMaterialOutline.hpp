#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"

namespace wde::scene {
	class ColorMaterialOutline : public Material {
		public:
			explicit ColorMaterialOutline(RenderStage stage)
				: Material("Color Material Outlined", stage, "color/colorShader.vert", "color/colorShader.frag", VK_POLYGON_MODE_LINE) {}

			// Serializers
			json serialize() override {
				return json({
					{"type", _materialName},
					{"stage", {
						{"first", _stage.first},
						{"second", _stage.second}
					}}
				});
			}
	};
}

