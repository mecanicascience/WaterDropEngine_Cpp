#pragma once

#include "../WdeRenderingStructures/objects/GameObject.hpp"
#include "InputManager.hpp"

using namespace wde::renderStructures;
using namespace wde::inputManager;

namespace wde::inputManager {
	class InputMovementController {
		public:
			void moveInPlaneXZ(float dt, GameObject &gameObject);

		private:
			float _moveSpeed {1.3f};
			float _lookSpeed {1.5f};
	};
}