#include "Sphere.hpp"

bool Sphere::hit(const Ray &ray, double tMin, double tMax, HitRecord &lastHitRecord) const {
	Vector originToCenter = ray.getFrom() - _center;
	auto a = ray.getDir().magSquared();
	auto halfB = originToCenter * ray.getDir();
	auto c = originToCenter.magSquared() - _radius*_radius;

	auto discriminant = halfB*halfB - a*c;
	if (discriminant < 0)
		return false;
	auto sqrtd = std::sqrt(discriminant);

	// Find nearest root in acceptable range
	auto root = (-halfB - sqrtd) / a;
	if (root < tMin || tMax < root) {
		root = (-halfB + sqrtd) / a;
		if (root < tMin || tMax < root)
			return false; // no root
	}

	// Update hit record
	lastHitRecord.t = root;
	lastHitRecord.point = ray.at(lastHitRecord.t);
	lastHitRecord.normal = (lastHitRecord.point - _center) / _radius;
	return true; // root found
}
