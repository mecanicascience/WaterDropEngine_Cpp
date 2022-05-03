#include "RayTracer.hpp"
#include "../materials/LambertianMaterial.hpp"
#include "../materials/MetalMaterial.hpp"
#include "../materials/DieletricMaterial.hpp"

namespace rtx {
	RayTracer::RayTracer() {
		// World materials
		auto materialGround = std::make_shared<LambertianMaterial>(Color(0.8, 0.8, 0.0));
		auto materialCenter = std::make_shared<LambertianMaterial>(Color(0.1, 0.2, 0.5));
		auto materialLeft   = std::make_shared<DielectricMaterial>(1.5);
		auto materialRight  = std::make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2));

		// World objects
		_worldObjects.add(make_shared<Sphere>(Vector( 0.0, -100.5, 2.2), 100.0, materialGround));
		_worldObjects.add(make_shared<Sphere>(Vector( 0.0,    0.0, 2.8),   0.5, materialCenter));
		_worldObjects.add(make_shared<Sphere>(Vector(-0.4,    0.0, 2.2),   0.5, materialLeft));
		_worldObjects.add(make_shared<Sphere>(Vector( 1.1,    0.0, 2.0),   0.5, materialRight));

		// Random small spheres
		int sphCount = 7;
		for (int a = -sphCount; a < sphCount; a++) {
			for (int b = -sphCount; b < sphCount; b++) {
				auto choose_mat = randomDouble();
				Vector center (a/20.0 + 0.8 * randomDouble(), -0.08, b/20.0 + 0.8 * randomDouble());
				std::shared_ptr<Material> sphereMaterial;

				if (choose_mat < 0.8) {
					// Diffuse
					Color albedo = Color { randomDouble()*randomDouble(), randomDouble()*randomDouble(), randomDouble()*randomDouble() };
					sphereMaterial = std::make_shared<LambertianMaterial>(albedo);
					_worldObjects.add(make_shared<Sphere>(center, 0.01, sphereMaterial));
				}
				else if (choose_mat < 0.95) {
					// Metal
					auto albedo = Color { randomDouble(0.5, 1), randomDouble(0.5, 1), randomDouble(0.5, 1) };
					sphereMaterial = std::make_shared<MetalMaterial>(albedo);
					_worldObjects.add(std::make_shared<Sphere>(center, 0.01, sphereMaterial));
				}
				else {
					// Glass
					sphereMaterial = std::make_shared<DielectricMaterial>(1.5);
					_worldObjects.add(make_shared<Sphere>(center, 0.01, sphereMaterial));
				}
			}
		}
	}

	Color RayTracer::getRayColor(const Ray &ray, int depth) {
		// Maximum ray depth
		if (depth <= 0)
			return {0, 0, 0};

		// Go until collision
		HitRecord rec {};
		if (_worldObjects.hit(ray, 0, infinity, rec)) {
			Ray scatteredRay {};
			Color attenuation {0, 0, 0};

			// Compute material scattering and diffusing at collision point
			if (rec.matPtr->scatter(ray, rec, attenuation, scatteredRay))
				return attenuation * getRayColor(scatteredRay, depth - 1); // Compute ray scattering color
			return {0, 0, 0};
		}

		// Add atmosphere
		float tGrad = (float) (0.5 * (ray.getDir().y / ray.getDir().mag() + 1.0));
		return Color {1, 1, 1} * (1.0f - tGrad) + Color {tGrad * 0.5, tGrad * 0.7, tGrad * 1.0};
	}
}
