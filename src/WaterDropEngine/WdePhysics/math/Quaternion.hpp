#pragma once

#include "Vector3.hpp"
#include <cfloat>

namespace wde::physics {
	/**
	 * Represents a qiaternopn
	 */
	class Quaternion {
		public:
			union {
				struct {
					/** Real component */
					double r;
					/** First complex component */
					double i;
					/** Second complex component */
					double j;
					/** Third complex component */
					double k;
				};

				/** Holds the quaternion data in array form */
				double data[4] {};
			};

			/** Create a default quaternion representing a 0 rotation. */
			Quaternion() : r(1), i(0), j(0), k(0) {}

			/**
			 * Create a new quaternion.
			 * @param r The real component of the quaternion.
			 * @param i The first complex component of the quaternion.
			 * @param j The second complex component of the quaternion.
			 * @param k The third complex component of the quaternion.
			 */
			Quaternion(const double r, const double i, const double j, const double k)
					: r(r), i(i), j(j), k(k) {}

			/** Normalises the quaternion to unit length, to give a valid orientation-quaternion. */
			void normalise() {
				double d = r*r+i*i+j*j+k*k; // Norm
				if (d < DBL_EPSILON) {
					r = 1;
					return;
				}

				d = ((double)1.0)/sqrt(d);
				r *= d;
				i *= d;
				j *= d;
				k *= d;
			}

			/**
			 * Rotate the Quaternion by a given angle.
			 * @param vector
			 */
			void rotateBy(const Vector3& vector) {
				Quaternion q(0, vector.x, vector.y, vector.z);
				(*this) *= q;
			}


			// Operators
			void operator +=(const Vector3 &vec) {
				r += r * ((double)0.5);
				i += i * ((double)0.5);
				j += j * ((double)0.5);
				k += k * ((double)0.5);
			}

			Quaternion operator +(const Vector3 &vec) const {
				return {
					r + r * ((double)0.5),
					i + i * ((double)0.5),
					j + j * ((double)0.5),
					k + k * ((double)0.5)
				};
			}

			void operator *=(const Quaternion &multiplier) {
				Quaternion q = *this;
				r = q.r*multiplier.r - q.i*multiplier.i -
				    q.j*multiplier.j - q.k*multiplier.k;
				i = q.r*multiplier.i + q.i*multiplier.r +
				    q.j*multiplier.k - q.k*multiplier.j;
				j = q.r*multiplier.j + q.j*multiplier.r +
				    q.k*multiplier.i - q.i*multiplier.k;
				k = q.r*multiplier.k + q.k*multiplier.r +
				    q.i*multiplier.j - q.j*multiplier.i;
			}

			Quaternion operator *(const Quaternion &multiplier) const {
				return {
					r*multiplier.r - i*multiplier.i -
				    j*multiplier.j - k*multiplier.k,
					r*multiplier.i + i*multiplier.r +
					j*multiplier.k - k*multiplier.j,
					r*multiplier.j + j*multiplier.r +
					k*multiplier.i - i*multiplier.k,
					r*multiplier.k + k*multiplier.r +
					i*multiplier.j - j*multiplier.i
				};
			}
	};
}
