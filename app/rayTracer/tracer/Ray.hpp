#pragma once

#include "glm/vec3.hpp"

using namespace glm;

/**
 * Represents a simple propagating ray
 */
class Ray {
	public:
		// Constructors
		explicit Ray() {}
		explicit Ray(const dvec3& from, const dvec3& dir) : _from(from), _dir(dir) {}


		// Getters and setters
		dvec3 getFrom() const { return _from; }
		dvec3 getDir() const { return _dir; }

		/**
		 * Return the ray coordinates at a path percentage
		 * @param t Percentage in the path
		 * @return A 3d vector
		 */
		dvec3 at(double t) const {
			return _from + t * _dir;
		}

	private:
		dvec3 _from;
		dvec3 _dir;
};
