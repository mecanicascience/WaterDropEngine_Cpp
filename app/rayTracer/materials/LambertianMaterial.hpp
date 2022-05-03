#pragma once

#include "Material.hpp"
#include "../utils/HitConstants.hpp"
#include "../objects/Sphere.hpp"

using namespace wde;

namespace rtx {
	class LambertianMaterial : public Material {
		public:
			explicit LambertianMaterial(Color albedo) : _albedo(albedo) {}

			bool scatter(const Ray& inputRay, const HitRecord& lastHitRecord, Color& attenuation, Ray& scatteredRay) const override {
				auto randomVector = Vector::randomInUnitSphere();
				auto scatterDirection = lastHitRecord.normal + randomVector / randomVector.mag();

				// Catch degenerate scatter direction
				if (scatterDirection.nearZero())
					scatterDirection = lastHitRecord.normal;

				scatteredRay = Ray (lastHitRecord.point, scatterDirection);
				attenuation = _albedo;
				return true;
			}

		private:
			Color _albedo;
	};
}
