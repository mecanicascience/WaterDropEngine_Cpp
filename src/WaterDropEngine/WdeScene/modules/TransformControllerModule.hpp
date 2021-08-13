#pragma once

#include "Module.hpp"
#include "../../WdeGUI/GUITheme.hpp"
#include "../../WdeGUI/GUIRenderer.hpp"

namespace wde::scene {
	class TransformControllerModule : public Module {
		public:
			// Constructors
			explicit TransformControllerModule(GameObject &gameObject);

			// Core functions
			void update(float deltaTime) override;
			void renderGUI() override;


			// Serializers
			/** Serialize the module */
			json serialize() override;
			/** Deserialize the module */
			void deserialize(json data) override;


		private:
			// Movement speed
			float _moveSpeed {1.3f};
			float _lookSpeed {1.5f};
	};
}

