#pragma once

#include "Hittable.hpp"

class Sphere : public Hittable {
	public:
		explicit Sphere() : _center(), _radius(0), Hittable() {}
		explicit Sphere(const Vector& center, double radius) : _center(center), _radius(radius), Hittable() {}

		// Compute methods
		bool hit(const Ray& ray, double tMin, double tMax, HitRecord& lastHitRecord) const override;

	private:
		Vector _center;
		double _radius;
};
