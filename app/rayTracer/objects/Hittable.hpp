#pragma once

#include "../utils/HitConstants.hpp"

/**
 * Stores the last hit data
 */
struct HitRecord {
	/** Intersection point */
	Vector point {};
	/** Intersection point normal */
	Vector normal {};
	/** Percentage of the vectr where it hits */
	double t = 0;
	/** True if the ray is coming from outside, false if coming from inside */
	bool frontFace;

	/**
	 * Set the face normal (keep inside or outside normal based on where the ray comes)
	 * @param ray Ray
	 * @param outwardNormal Local object normal at intersection point
	 */
	inline void setFaceNormal(const Ray& ray, const Vector& outwardNormal) {
		frontFace = (ray.getDir() * outwardNormal) < 0;
		normal = frontFace ? outwardNormal : outwardNormal * (-1);
	}
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
