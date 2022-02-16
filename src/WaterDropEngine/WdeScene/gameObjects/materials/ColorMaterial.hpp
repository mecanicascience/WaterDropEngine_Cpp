#pragma once

#include "Material.hpp"

namespace wde::scene {
	class ColorMaterial : public Material {
		public:
			explicit ColorMaterial() : Material("Color Material") {}
	};
}

