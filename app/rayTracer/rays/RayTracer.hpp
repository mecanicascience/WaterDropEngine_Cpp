#pragma once

#include "Ray.hpp"
#include "../../../src/WaterDropEngine/WdeCommon/WdeUtils/Color.hpp"
#include "../objects/Sphere.hpp"

using namespace wde;

class RayTracer {
	public:
		explicit RayTracer();

		/**
		 * @param ray The input ray to compute color
		 * @return The ray color
		 */
		Color getRayColor(const Ray& ray);
};
