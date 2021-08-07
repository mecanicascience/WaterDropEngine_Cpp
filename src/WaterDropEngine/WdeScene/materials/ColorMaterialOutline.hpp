#pragma once

#include <vulkan/vulkan_core.h>

namespace wde::scene {
	class ColorMaterialOutline : public Material {
	public:
		explicit ColorMaterialOutline(RenderStage stage)
			: Material("Color Material Outlined", stage, "colorShader.vert", "colorShader.frag", VK_POLYGON_MODE_LINE) {}
	};
}

