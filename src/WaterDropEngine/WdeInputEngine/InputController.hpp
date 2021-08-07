#pragma once

#include <cmath>

#include "../WdeScene/objects/GameObject.hpp"
#include "InputManager.hpp"

using namespace wde::scene;
using namespace wde::inputManager;

namespace wde::inputManager {
	class InputController {
		public:
			/**
			 * Move the game object in the XZ plane
			 * @param dt
			 * @param gameObject
			 * @param moveSpeed
			 * @param lookSpeed
			 */
			static void moveInPlaneXZ(float dt, GameObject &gameObject, float moveSpeed = 1.3f, float lookSpeed = 1.5f);
	};
}
