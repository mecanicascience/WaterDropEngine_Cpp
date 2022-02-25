#pragma once

#include "Module.hpp"
#include "../../../WdeGUI/GUIRenderer.hpp"
#include "../../../WdeInput/InputController.hpp"

namespace wde::scene {
	class ControllerModule : public Module {
		public:
			// Constructors
			explicit ControllerModule(GameObject &gameObject) : Module(gameObject, "Keyboard Controller") {}

			// Core functions
			void tick() override {
				if (!_gameObject.isSelected())
					return;

				auto newTime = std::chrono::steady_clock::now();
				auto deltaTime =
						std::chrono::time_point_cast<std::chrono::milliseconds>(newTime).time_since_epoch()
						- std::chrono::time_point_cast<std::chrono::milliseconds>(_lastTime).time_since_epoch();
				_lastTime = newTime;

				// Move in plane XZ given the keyboard inputs
				input::InputController::moveInPlaneXZ((float) deltaTime.count() / 1000.0f, _gameObject, _moveSpeed, _lookSpeed);
			}

			void drawGUI() override {
				// Move speed
				gui::GUIRenderer::addFloatDragger("Move speed", _moveSpeed, 1.3f);
				// Look speed
				gui::GUIRenderer::addFloatDragger("Look speed", _lookSpeed, 1.5f);
			}


		private:
			// Class data
			std::chrono::time_point<std::chrono::steady_clock> _lastTime = std::chrono::steady_clock::now();

			// Movement speed
			float _moveSpeed {20.0f};
			float _lookSpeed {1.5f};
	};
}

