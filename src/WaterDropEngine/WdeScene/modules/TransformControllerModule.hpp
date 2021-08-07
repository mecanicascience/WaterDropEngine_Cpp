#pragma once

#include "Module.hpp"
#include "../../WdeGUI/GUITheme.hpp"
#include "../../WdeGUI/GUIRenderer.hpp"
#include "../../WdeInputEngine/InputController.hpp"

namespace wde::scene {
	class TransformControllerModule : public Module {
		public:
			// Constructors
			explicit TransformControllerModule(GameObject &gameObject) : Module(gameObject, "Transform Controler") {}

			// Core functions
			void update(float deltaTime) override {
				// Move in plane XZ given the keyboard inputs
				InputController::moveInPlaneXZ(deltaTime, _gameObject, _moveSpeed, _lookSpeed);
			}

			void renderGUI() override {
				// Move speed
				gui::GUIRenderer::addFloatDragger("Move speed", _moveSpeed, 1.3f);
				// Look speed
				gui::GUIRenderer::addFloatDragger("Look speed", _lookSpeed, 1.5f);
			}


		private:
			// Movement speed
			float _moveSpeed {1.3f};
			float _lookSpeed {1.5f};
	};
}

