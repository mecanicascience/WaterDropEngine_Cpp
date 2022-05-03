#include "RayTracer.hpp"

RayTracer::RayTracer() {
	// World spheres
	_worldObjects.add(std::make_shared<Sphere>(Vector(0, 0, 2), 0.5));
	_worldObjects.add(std::make_shared<Sphere>(Vector(0, -100.5, -1), 100));
}

Color RayTracer::getRayColor(const Ray& ray, int depth) {
	// Maximum ray depth
    if (depth <= 0)
        return {0, 0, 0};

	// Parameters
	float diffusionCoefficient = 0.5;

	HitRecord rec {};
	if (_worldObjects.hit(ray, 0, infinity, rec)) {
		// Simple diffusion along the normal vector, no scattering
		Vector target = rec.point + rec.normal + Vector::randomInHemisphere(rec.normal);
		auto col = getRayColor(Ray {rec.point, target - rec.point}, depth - 1);
		return {col._r * diffusionCoefficient, col._g * diffusionCoefficient, col._b * diffusionCoefficient, col._a * diffusionCoefficient};
	}

	// Add atmosphere
	double tGrad = 0.5 * (ray.getDir().y / ray.getDir().mag() + 1.0);
	return {1.0 - tGrad + tGrad * 0.5, 1.0 - tGrad + tGrad * 0.7, 1.0 - tGrad + tGrad * 1.0};
}


