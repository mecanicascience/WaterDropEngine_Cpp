#pragma once

#include "Material.hpp"

namespace rtx {
	class DielectricMaterial : public Material {
		public:
			explicit DielectricMaterial(double n) : _n(n) {}

			// Ray direction compute
			bool scatter(const Ray& inputRay, const HitRecord& lastHitRecord, Color& attenuation, Ray& scatteredRay) const override {
				attenuation = {1, 1, 1}; // No attenuation

				// Assume from or to air refraction
				double refractionRatio = lastHitRecord.frontFace ? (1.0 / _n) : (_n / 1.0);

				// Compute util math methods
				Vector unitDirection = inputRay.getDir() / inputRay.getDir().mag();
				double cosTheta = fmin((unitDirection * (-1)) * lastHitRecord.normal, 1.0);
				double sinTheta = std::sqrt(1.0 - cosTheta*cosTheta);

				// No solutions to Snell laws
				bool cannotRefract = refractionRatio * sinTheta > 1.0;
				Vector direction;
				if (cannotRefract || shouldReflect(cosTheta, refractionRatio) > randomDouble())
					direction = reflect(unitDirection, lastHitRecord.normal);
				else
					direction = refract(unitDirection, lastHitRecord.normal, refractionRatio);

				scatteredRay = Ray {lastHitRecord.point, direction};
				return true;
			}

		private:
			/** Refraction index */
			double _n;

			/**
			 * Compute the reflection versus transmission ratio
			 * @param cosVal Angle cos with the normal
			 * @param refN Material refraction index
			 * @return The ration of reflection versus transmission
			 */
			static double shouldReflect(double cosVal, double refN) {
				// Using Schlick's approximation
				auto r0 = (1 - refN) / (1 + refN);
				r0 = r0 * r0;
				return r0 + (1 - r0) * std::pow((1 - cosVal), 5);
			}
	};
}
