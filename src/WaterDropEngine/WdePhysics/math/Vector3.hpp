#pragma once

#include "../../WdeCommon/WdeException/WdeException.hpp"

#include <cmath>

namespace wde::physics {
	/**
	 * Represents a 3D-Vector
	 */
	class Vector3 {
		public:
			// Values
			double x;
			double y;
			double z;

		private:
			// Padding to ensure 4 word alignment
			double pad = 0;

		public:
			/** Creates a null vector */
			Vector3() : x(0), y(0), z(0) {}

			/**
			 * Creates a new vector
			 * @param x
			 * @param y
			 * @param z
			 */
			Vector3(const double x, const double y, const double z) : x(x), y(y), z(z) {}


			// Core functions
			/** Compute the cross product of this vector and another */
			Vector3 cross(const Vector3 &vector) const {
				return { y*vector.z - z*vector.y,
				         z*vector.x - x*vector.z,
				         x*vector.y - y*vector.x};
			}

			/** Compute the scalar product of this vector and another */
			double dot(const Vector3 &vector) const {
				return x*vector.x + y*vector.y + z*vector.z;
			}

			/** Gets the magnitude of this vector. */
			double mag() const {
				return sqrt(x*x + y*y + z*z);
			}

			/** Get the square magnitude of this vector */
			double squareMag() const {
				return x*x + y*y + z*z;
			}

			/** Limits the size of the vector to the given maximum */
			void trim(double size) {
				if (squareMag() > size*size) {
					normalise();
					x *= size;
					y *= size;
					z *= size;
				}
			}

			/** Normalize this vector */
			void normalise() {
				double l = mag();
				if (l > 0) {
					(*this) *= ((double)1) / l;
				}
			}

			/** Clears vector components */
			void clear() {
				x = y = z = 0;
			}

			/** Flips all the components of the vector */
			void invert() {
				x = -x;
				y = -y;
				z = -z;
			}


			// Operators
			bool operator==(const Vector3& other) const {
				return x == other.x &&
				       y == other.y &&
				       z == other.z;
			}

			bool operator!=(const Vector3& other) const {
				return !(*this == other);
			}

			double operator[](unsigned i) const {
				if (i == 0) return x;
				if (i == 1) return y;
				if (i == 2) return z;
				throw WdeException(LogChannel::PHYS, "Trying to access component " + std::to_string(i) + " of a 3D-Vector.");
			}

			double& operator[](unsigned i) {
				if (i == 0) return x;
				if (i == 1) return y;
				if (i == 2) return z;
				throw WdeException(LogChannel::PHYS, "Trying to access component " + std::to_string(i) + " of a 3D-Vector.");
			}

			void operator+=(const Vector3& v) {
				x += v.x;
				y += v.y;
				z += v.z;
			}

			Vector3 operator+(const Vector3& v) const {
				return { x+v.x, y+v.y, z+v.z };
			}

			void operator-=(const Vector3& v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
			}

			Vector3 operator-(const Vector3& v) const {
				return { x-v.x, y-v.y, z-v.z };
			}

			/** Scalar product between two vectors */
			void operator*=(const double val) {
				x *= val;
				y *= val;
				z *= val;
			}

			/** Scalar product between two vectors */
			Vector3 operator*(const double value) const {
				return { x*value, y*value, z*value };
			}

			/** Scalar product between this vector and another */
			double operator*(const Vector3 &vector) const {
				return x*vector.x + y*vector.y + z*vector.z;
			}

			/** Cross product between two vectors */
			void operator %=(const Vector3 &vector) {
				*this = Vector3(y*vector.z - z*vector.y,
				                z*vector.x - x*vector.z,
				                x*vector.y - y*vector.x);
			}

			/** Cross product between this vector and another */
			Vector3 operator%(const Vector3 &vector) const {
				return { y*vector.z - z*vector.y,
				         z*vector.x - x*vector.z,
				         x*vector.y - y*vector.x };
			}
	};
}
