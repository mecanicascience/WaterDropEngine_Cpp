#pragma once

#include <utility>

#include "Hittable.hpp"

/*
 * Represents a list of hittable objects
 */
class HittableList : public Hittable {
	public:
		HittableList() = default;
		/** Create a new hittable list of objects given a hittable list of vectors */
		explicit HittableList(std::vector<std::shared_ptr<Hittable>> object) : _objects(std::move(object)) {}


		// Object manager methods
		/** Clear the objects lists */
		void clear() { _objects.clear(); }
		/**
		 * Ajoute un objet à la liste
		 * @param object
		 */
		void add(const std::shared_ptr<Hittable>& object) { _objects.push_back(object); }


		// Hit methods
		bool hit(const Ray& r, double tMin, double tMax, HitRecord& lastHitRecord) const override {
			HitRecord tempRec;
			bool hitAnything = false;
			auto closestSoFar = tMax;

			for (const auto& object : _objects) {
				if (object->hit(r, tMin, closestSoFar, tempRec)) {
					hitAnything = true;
					closestSoFar = tempRec.t;
					lastHitRecord = tempRec;
				}
			}

			return hitAnything;
		}


	public:
		std::vector<std::shared_ptr<Hittable>> _objects;
};
