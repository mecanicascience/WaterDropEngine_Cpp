#pragma once

#include "Ray.hpp"
#include "../../../src/WaterDropEngine/WdeCommon/WdeUtils/Color.hpp"
#include "../objects/Sphere.hpp"
#include "../utils/HitConstants.hpp"
#include "../objects/HittableList.hpp"

using namespace wde;


namespace rtx {
	class RayTracer {
		public:
			explicit RayTracer();

			/**
			 * Compute the ray color
			 * @param ray The input ray to compute color
			 * @param depth Current ray depth
			 * @return The ray color
			 */
			Color getRayColor(const Ray &ray, int depth);


		private:
			HittableList _worldObjects{};
	};
}
