#pragma once

#include "../utils/Vector.hpp"
#include "../rays/Ray.hpp"

/**
 * Stores the last hit data
 */
struct HitRecord {
	Vector point {};
	Vector normal {};
	double t = 0;
};

/**
 * Represents an object ray-hittable object
 */
class Hittable {
	public:
		/**
		 * Compute if the ray hits the object
		 * @param ray The incoming ray
		 * @param tMin Minimum ray t to read from
		 * @param tMax Maximum ray t to read to
		 * @param lastHitRecord The last hit record data
		 * @return True if the ray hits the object
		 */
		virtual bool hit(const Ray& ray, double tMin, double tMax, HitRecord& lastHitRecord) const = 0;
};
