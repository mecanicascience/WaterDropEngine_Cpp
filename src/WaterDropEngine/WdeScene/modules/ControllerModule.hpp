#pragma once

#include "Module.hpp"
#include "../../WdeGUI/GUIRenderer.hpp"
#include "../../WdeInput/InputController.hpp"

namespace wde::scene {
	class ControllerModule : public Module {
		public:
			// Constructors
			explicit ControllerModule(GameObject &gameObject);
			explicit ControllerModule(GameObject &gameObject, const std::string& data);

			// Core functions
			void tick() override;
			void drawGUI() override;


		private:
			// Class data
			std::chrono::time_point<std::chrono::steady_clock> _lastTime = std::chrono::steady_clock::now();

			// Movement speed
			float _moveSpeed {20.0f};
			float _lookSpeed {1.5f};
	};
}

