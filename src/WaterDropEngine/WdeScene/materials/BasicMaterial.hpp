#pragma once

namespace wde::scene {
	class BasicMaterial : public Material {
		public:
			BasicMaterial(RenderStage stage)
			  : Material(
			  		"Basic Material", stage,
			  		"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv",
			  		VK_POLYGON_MODE_FILL) {}
	};
}

