#include "InputController.hpp"
#include "../WaterDropEngine.hpp"

#include <cmath>
#include <glm/gtc/constants.hpp>

namespace wde::input {
	void InputController::moveInPlaneXZ(float dt, scene::GameObject &gameObject, float moveSpeed, float lookSpeed) {
		WDE_PROFILE_FUNCTION();
		// Update game objects rotation
		glm::vec3 rotation {0.0f};

		if (WaterDropEngine::get().getInput().isKeyDown("lookUp"))    rotation.x -= 1.0f;
		if (WaterDropEngine::get().getInput().isKeyDown("lookDown"))  rotation.x += 1.0f;
		if (WaterDropEngine::get().getInput().isKeyDown("lookRight")) rotation.y += 1.0f;
		if (WaterDropEngine::get().getInput().isKeyDown("lookLeft"))  rotation.y -= 1.0f;

		auto& transformModule = gameObject.transform;
		if (glm::dot(rotation, rotation) > glm::epsilon<float>()) { // If rotation not negligible
			transformModule->rotation += lookSpeed * dt * glm::normalize(rotation);
		}

		// Mod rotations by 2 pi
		transformModule->rotation.x = glm::mod(transformModule->rotation.x, glm::two_pi<float>());
		transformModule->rotation.y = glm::mod(transformModule->rotation.y, glm::two_pi<float>());

		// Compute gameObject local axis vectors
		const float sYaw = std::sin(transformModule->rotation.y);
		const float cYaw = std::cos(transformModule->rotation.y);
		const glm::vec3 forwardDirection {sYaw, 0.0f,  cYaw};
		const glm::vec3 rightDirection   {cYaw, 0.0f, -sYaw}; // Right = Forward + Pi/2, in XZ plane
		const glm::vec3 upDirection {0.0f,  1.0f, 0.0f};

		// Move in requested direction
		glm::vec3 moveDirection {0.0f};
		if (WaterDropEngine::get().getInput().isKeyDown("moveForward"))  moveDirection += forwardDirection;
		if (WaterDropEngine::get().getInput().isKeyDown("moveBackward")) moveDirection -= forwardDirection;
		if (WaterDropEngine::get().getInput().isKeyDown("moveRight"))    moveDirection += rightDirection;
		if (WaterDropEngine::get().getInput().isKeyDown("moveLeft"))     moveDirection -= rightDirection;
		if (WaterDropEngine::get().getInput().isKeyDown("moveUp"))       moveDirection += upDirection;
		if (WaterDropEngine::get().getInput().isKeyDown("moveDown"))     moveDirection -= upDirection;

		if (glm::dot(moveDirection, moveDirection) > glm::epsilon<float>()) // If rotation not negligible
			transformModule->position += moveSpeed * dt * glm::normalize(moveDirection);
	}
}
