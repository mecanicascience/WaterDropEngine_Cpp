#pragma once

#include "../utils/HitConstants.hpp"
#include "../objects/Hittable.hpp"
#include "../../../src/WaterDropEngine/WdeCommon/WdeUtils/Color.hpp"

using namespace wde;

namespace rtx {
	class HitRecord;

	class Material {
		public:
			/**
			 * Scatter an incoming ray through the material
			 * @param inputRay Input ray
			 * @param lastHitRecord Last ray touched place
			 * @param attenuation Attenuation amount
			 * @param scatteredRay The output scattered ray
			 * @return True if the ray has been scattered
			 */
			virtual bool scatter(const Ray& inputRay, const HitRecord& lastHitRecord, Color& attenuation, Ray& scatteredRay) const = 0;


		protected:
			/**
			 * Reflects a vector
			 * @param vIn Input ray
			 * @param surfaceNormal Local surface normal
			 * @return The reflected ray
			 */
			static Vector reflect(const Vector& vIn, const Vector& surfaceNormal) {
				return vIn - 2 * (vIn * surfaceNormal) * surfaceNormal;
			}

			/**
			 * Compute a refraction of a ray
			 * @param vIn Input ray
			 * @param n Initial refraction index n
			 * @param nOvern The ratio of the refraction indices n/n'
			 * @return The refracted ray
			 */
			static Vector refract(const Vector& vIn, const Vector& n, double nOvern) {
				auto cosTheta = std::fmin(vIn * (-1) * n, 1.0);
				Vector rOutPerp =  nOvern * (vIn + cosTheta * n);
				Vector rOutParal = -sqrt(fabs(1.0 - rOutPerp.magSquared())) * n;
				return rOutPerp + rOutParal;
			}
	};
}
