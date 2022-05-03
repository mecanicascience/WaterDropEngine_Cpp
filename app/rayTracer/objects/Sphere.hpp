#pragma once

#include <utility>

#include "Hittable.hpp"

namespace rtx {
	class Sphere : public Hittable {
		public:
			explicit Sphere() : _center(), _radius(0), Hittable() { }

			explicit Sphere(const Vector &center, double radius, std::shared_ptr<Material> material) : _center(center), _radius(radius), _material(std::move(material)), Hittable() { }

			// Compute methods
			bool hit(const Ray &ray, double tMin, double tMax, HitRecord &lastHitRecord) const override;

		private:
			Vector _center;
			double _radius;
			std::shared_ptr<Material> _material;
	};
}
