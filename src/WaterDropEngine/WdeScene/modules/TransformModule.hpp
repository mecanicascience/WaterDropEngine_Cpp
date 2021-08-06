#pragma once

#include "Module.hpp"
#include "../../WdeGUI/GUITheme.hpp"

#include "../../WdeGUI/GUIRenderer.hpp"

namespace wde::scene {
	class TransformModule : public Module {
		public:
			explicit TransformModule() : Module("Transform") {}

			void renderGUI() override {
				gui::GUIRenderer::createVec3Button("Position", _position);
				gui::GUIRenderer::createVec3Button("Rotation", _rotation);
				gui::GUIRenderer::createVec3Button("Scale", _scale, 1.0f);
			}

		private:
			glm::vec3 _position {0.0f, 0.0f, 0.0f};
			glm::vec3 _rotation {0.0f, 0.0f, 0.0f};
			glm::vec3 _scale {1.0f, 1.0f, 1.0f};
	};
}

