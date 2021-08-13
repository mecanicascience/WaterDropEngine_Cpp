#include "TransformControllerModule.hpp"
#include "../../WdeInputEngine/InputController.hpp"

namespace wde::scene {
	// Constructors
	TransformControllerModule::TransformControllerModule(GameObject &gameObject) : Module(gameObject, "Transform Controller") {}

	// Core functions
	void TransformControllerModule::update(float deltaTime) {
		// Move in plane XZ given the keyboard inputs
		InputController::moveInPlaneXZ(deltaTime, _gameObject, _moveSpeed, _lookSpeed);
	}

	void TransformControllerModule::renderGUI() {
		// Move speed
		gui::GUIRenderer::addFloatDragger("Move speed", _moveSpeed, 1.3f);
		// Look speed
		gui::GUIRenderer::addFloatDragger("Look speed", _lookSpeed, 1.5f);
	}


	// Serializers
	json TransformControllerModule::serialize() {
		return json({
			{"moveSpeed", _moveSpeed},
			{"lookSpeed", _lookSpeed}
		});
	}

	void TransformControllerModule::deserialize(json data) {
		_moveSpeed = float(data["moveSpeed"]);
		_lookSpeed = float(data["lookSpeed"]);
	}
}
