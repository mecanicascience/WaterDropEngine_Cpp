#pragma once

#include <cassert>
#include <cfloat>
#include "../math/Vector3.hpp"

namespace wde::physics {
	/**
	 * A simple particle
	 */
	class Particle {
		protected:
			/** Particle position */
			Vector3 _pos;
			/** Particle velocity */
			Vector3 _vel;
			/** Particle acceleration */
			Vector3 _acc;

			/** Damping percentage to compensate numerical integration errors (default 1 for no damping) */
			double _damping = 1;
			/** Inverse mass of particle (0 for immovable) */
			double _inverseMass = 0;
			/** Internal force accumulator */
			Vector3 _forceAccumulator;

		public:
			// Core physics methods
			/**
			 * Integrate system based on given forces, then clear forces.
			 * Using Euler method.
			 * @param dt
			 */
			void integrate(double dt);

			// Getters and setters
			void setMass(const double mass) {
				assert(mass != 0);
				_inverseMass = ((double) 1.0) / mass;
			}
			double getMass() const {
				if (_inverseMass == 0)
					return DBL_MAX;
				return ((double)1.0) / _inverseMass;
			}
			void setInverseMass(const double inverseMass) { _inverseMass = inverseMass; }
			double getInverseMass() const { return _inverseMass; }
			bool hasFiniteMass() const { return _inverseMass >= 0.0f; }

			void setDamping(const double damping) { _damping = damping; }
			double getDamping() const { return _damping; }

			void setPosition(const Vector3 &position) { _pos = position; }
			void setPosition(const double x, const double y, const double z) {
				_pos.x = x;
				_pos.y = y;
				_pos.z = z;
			}
			Vector3 getPosition() const { return _pos; }

			void setVelocity(const Vector3 &velocity) { _vel = velocity; }
			void setVelocity(const double x, const double y, const double z) {
				_vel.x = x;
				_vel.y = y;
				_vel.z = z;
			}
			Vector3 getVelocity() const { return _vel; }

			void setAcceleration(const Vector3 &acceleration) { _acc = acceleration; }
			void setAcceleration(const double x, const double y, const double z) {
				_acc.x = x;
				_acc.y = y;
				_acc.z = z;
			}
			void getAcceleration(Vector3 *acceleration) const { *acceleration = acceleration; }
			Vector3 getAcceleration() const { return _acc; }

			void clearForces() { _forceAccumulator.clear(); }
			void addForce(const Vector3 &force) { _forceAccumulator += force; }
	};
}
