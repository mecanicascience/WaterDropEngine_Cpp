#pragma once

#include <stdexcept>
#include <cmath>
#include "../../../src/WaterDropEngine/WdeCommon/WdeException/WdeException.hpp"
#include "HitConstants.hpp"

namespace rtx {
	class Vector {
		public:
			double x;
			double y;
			double z;

			// Constructeurs
			Vector(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) { };

			Vector(const Vector &v) : x(v.x), y(v.y), z(v.z) { }


			// Util methods
			/** @return The vector magnitude */
			double mag() const {
				return std::sqrt(magSquared());
			}

			/** @return The vector squared magnitude */
			double magSquared() const {
				return x * x + y * y + z * z;
			}



			// Operators overload
			/** Scalar product */
			friend Vector operator*(double a, const Vector &v) {
				return {v.x * a, v.y * a, v.z * a};
			}


			Vector &operator/=(double a) {
				if (a == 0)
					throw wde::WdeException(wde::LogChannel::RENDER, "Cannot divide a Vector by 0.");
				x /= a;
				y /= a;
				z /= a;
				return *this;
			}

			Vector &operator*=(double a) {
				x *= a;
				y *= a;
				z *= a;
				return *this;
			}

			Vector &operator-=(const Vector &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}

			Vector &operator+=(const Vector &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}

			friend Vector operator/(const Vector &v, double a) {
				if (a == 0)
					throw wde::WdeException(wde::LogChannel::RENDER, "Cannot divide a Vector by 0.");
				return {v.x / a, v.y / a, v.z / a};
			}

			friend Vector operator/(double a, const Vector &v) {
				if (a == 0)
					throw wde::WdeException(wde::LogChannel::RENDER, "Cannot divide a Vector by 0.");
				return {v.x / a, v.y / a, v.z / a};
			}

			friend Vector operator*(const Vector &v, double a) {
				return {v.x * a, v.y * a, v.z * a};
			}

			friend double operator*(const Vector &v1, const Vector &v2) {
				return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
			}

			friend Vector operator-(const Vector &v1, const Vector &v2) {
				return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
			}

			friend Vector operator+(const Vector &v1, const Vector &v2) {
				return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
			}

			friend std::ostream &operator<<(std::ostream &os, const Vector &v) {
				os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
				return os;
			}


			// Random Vectors
			inline static Vector random() {
				return {randomDouble(), randomDouble(), randomDouble()};
			}

			inline static Vector random(double min, double max) {
				return {randomDouble(min, max), randomDouble(min, max), randomDouble(min, max)};
			}

			inline static Vector randomInUnitSphere() {
				while (true) {
					auto p = Vector::random(-1, 1);
					if (p.magSquared() >= 1) continue;
					return p;
				}
			}

			/**
			 * @param normal Normal vector that define the distribution
			 * @return A random vector given a normal distribution
			 */
			inline static Vector randomInHemisphere(const Vector &normal) {
				Vector inUnitSphere = randomInUnitSphere();
				if (inUnitSphere * normal > 0.0) // In the same hemisphere as the normal
					return inUnitSphere;
				else
					return inUnitSphere * (-1);
			}

			/** @return true if the vector is near 0 */
			bool nearZero() const {
				// Return true if the vector is close to zero in all dimensions.
				const auto s = 1e-8;
				return (std::fabs(x) < s) && (std::fabs(y) < s) && (std::fabs(z) < s);
			}
	};
}
