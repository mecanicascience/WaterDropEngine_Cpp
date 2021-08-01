#include "InputMovementController.hpp"

#include <cmath>

namespace wde::inputManager {
	void InputMovementController::moveInPlaneXZ(float dt, GameObject &gameObject) {
		// Update game objects rotation
		glm::vec3 rotation {0.0f};

		if (InputManager::get().isKeyDown("lookUp"))    rotation.x += 1.0f;
		if (InputManager::get().isKeyDown("lookDown"))  rotation.x -= 1.0f;
		if (InputManager::get().isKeyDown("lookRight")) rotation.y += 1.0f;
		if (InputManager::get().isKeyDown("lookLeft"))  rotation.y -= 1.0f;

		if (glm::dot(rotation, rotation) > glm::epsilon<float>()) { // If rotation not negligible
			gameObject.transform.rotation += _lookSpeed * dt * glm::normalize(rotation);
		}

		// Mod rotations by 2 pi
		gameObject.transform.rotation.x = glm::mod(gameObject.transform.rotation.x, glm::two_pi<float>());
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		// Compute gameObject local axis vectors
		const float sYaw = std::sin(gameObject.transform.rotation.y);
		const float cYaw = std::cos(gameObject.transform.rotation.y);
		const glm::vec3 forwardDirection {sYaw, 0.0f,  cYaw};
		const glm::vec3 rightDirection   {cYaw, 0.0f, -sYaw}; // Right = Forward + Pi/2, in XZ plane
		const glm::vec3 upDirection {0.0f, -1.0f, 0.0f};

		// Move in requested direction
		glm::vec3 moveDirection {0.0f};
		if (InputManager::get().isKeyDown("moveForward"))  moveDirection += forwardDirection;
		if (InputManager::get().isKeyDown("moveBackward")) moveDirection -= forwardDirection;
		if (InputManager::get().isKeyDown("moveRight"))    moveDirection += rightDirection;
		if (InputManager::get().isKeyDown("moveLeft"))     moveDirection -= rightDirection;
		if (InputManager::get().isKeyDown("moveUp"))       moveDirection += upDirection;
		if (InputManager::get().isKeyDown("moveDown"))     moveDirection -= upDirection;

		if (glm::dot(moveDirection, moveDirection) > glm::epsilon<float>()) { // If rotation not negligible
			gameObject.transform.translation += _moveSpeed * dt * glm::normalize(moveDirection);
		}
	}
}
