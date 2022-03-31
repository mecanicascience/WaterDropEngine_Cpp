#pragma once

#include "../../wde.hpp"
#include "../WdeScene/GameObject.hpp"

namespace wde::input {
	class InputController : public NonCopyable {
		public:
			/**
			 * Move the game object in the XZ plane
			 * @param dt
			 * @param gameObject
			 * @param moveSpeed
			 * @param lookSpeed
			 */
			static void moveInPlaneXZ(float dt, scene::GameObject &gameObject, float moveSpeed = 1.3f, float lookSpeed = 1.5f);
	};
}
