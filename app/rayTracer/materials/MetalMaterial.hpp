#pragma once

#include "Material.hpp"

namespace rtx {
	class MetalMaterial : public Material {
		public:
			explicit MetalMaterial(Color albedo) : _albedo(albedo) {}

			// Ray direction compute
			bool scatter(const Ray& inputRay, const HitRecord& lastHitRecord, Color& attenuation, Ray& scatteredRay) const override {
				Vector reflected = reflect(inputRay.getDir() / inputRay.getSize(), lastHitRecord.normal);
				scatteredRay = Ray {lastHitRecord.point, reflected};
				attenuation = _albedo;
				return ((scatteredRay.getDir() * lastHitRecord.normal) > 0);
			}

		private:
			Color _albedo;
	};
}
