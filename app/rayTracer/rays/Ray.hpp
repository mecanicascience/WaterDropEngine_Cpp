#pragma once

#include <cmath>
#include "../utils/Vector.hpp"

/**
 * Represents a simple propagating ray
 */
class Ray {
	public:
		// Constructors
		explicit Ray() : _from(), _dir() {}
		explicit Ray(const Vector& from, const Vector& dir) : _from(from), _dir(dir) {}


		// Getters and setters
		Vector getFrom() const { return _from; }
		Vector getDir() const { return _dir; }
		double getSize() const { return (_dir - _from).mag(); }

		/**
		 * Return the ray coordinates at a path percentage
		 * @param t Percentage in the path
		 * @return A 3d vector
		 */
		Vector at(double t) const {
			return _from + t * _dir;
		}

		friend std::ostream& operator<<(std::ostream &os, const Ray& r) {
			os << "[" << r._from << " - " << r._dir << "]";
			return os;
		}


	private:
		Vector _from;
		Vector _dir;
};
