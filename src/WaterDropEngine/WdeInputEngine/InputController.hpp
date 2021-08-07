#pragma once

#include "../WdeScene/objects/GameObject.hpp"
#include "InputManager.hpp"

using namespace wde::scene;
using namespace wde::inputManager;

namespace wde::inputManager {
	class InputController {
		public:
			void moveInPlaneXZ(float dt, GameObject &gameObject, float moveSpeed = 1.3f, float lookSpeed = 1.5f);
	};
}
