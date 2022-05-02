#include "RayTracer.hpp"


Sphere _sphere {{0, 0, 10}, 1};
RayTracer::RayTracer() {

}

Color RayTracer::getRayColor(const Ray& ray) {
	HitRecord rec {};
	if (_sphere.hit(ray, 0, 1, rec))
		return Color::RED;
	return Color::BLUE;
}
