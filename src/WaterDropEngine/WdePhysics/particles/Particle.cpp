#include "Particle.hpp"

namespace wde::physics {
	void Particle::integrate(double dt) {
		// If zero mass, don't integrate
		if (_inverseMass <= 0.0f)
			return;

		// Update position
		_pos += _vel * dt;

		// Compute acceleration
		Vector3 resultingAcc = _acc;
		_acc += _forceAccumulator * _inverseMass;

		// Compute velocity
		_vel += resultingAcc * dt;
		_vel *= pow(_damping, dt); // Drag

		// Reset forces
		clearForces();
	}
}
