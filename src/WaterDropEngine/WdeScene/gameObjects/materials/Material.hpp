#pragma once

#include "../../../../wde.hpp"

namespace wde::scene {
	class Material {
		public:
			explicit Material(const std::string& name) : _name(name) {}


			// GameObject materials
			/**
			 * Assign a new game object to the material
			 * @param goID GameObject ID
			 */
			void addGameObject(int goID) {
				_boundedGoIds.push_back(goID);
			}

			/**
			 * Remove a game object to the material
			 * @param goID GameObject ID
			 */
			void removeGameObject(int goID) {
				_boundedGoIds.erase(std::remove(_boundedGoIds.begin(), _boundedGoIds.end(), goID), _boundedGoIds.end());
			}


			// Getters and setters
			std::string getName() { return _name; }


		private:
			/** IDs of the game objects with mesh corresponding to this material */
			std::vector<int> _boundedGoIds {};
			/** Material name */
			std::string _name;
	};
}
