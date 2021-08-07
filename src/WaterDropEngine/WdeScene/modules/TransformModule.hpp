#pragma once

#include "Module.hpp"
#include "../../WdeGUI/GUITheme.hpp"
#include "../../WdeGUI/GUIRenderer.hpp"

namespace wde::scene {
	class TransformModule : public Module {
		public:
			// Core parameters
			/** The game object world position */
			glm::vec3 position {0.0f, 0.0f, 0.0f};
			/** The game object world axis rotation */
			glm::vec3 rotation {0.0f, 0.0f, 0.0f};
			/** The game object world scale */
			glm::vec3 scale {1.0f, 1.0f, 1.0f};


			// Constructors
			explicit TransformModule(GameObject &gameObject) : Module(gameObject, "Transform") {}

			// Core functions
			void renderGUI() override {
				gui::GUIRenderer::createVec3Button("Position", position);
				gui::GUIRenderer::createVec3Button("Rotation", rotation);
				gui::GUIRenderer::createVec3Button("Scale", scale, 1.0f);
			}

			// Getters and setters
			/** Return the corresponding transform matrix : Translation * Ry * Rx * Rz * scale */
			glm::mat4 getTransform() const {
				const float c3 = glm::cos(rotation.z);
				const float s3 = glm::sin(rotation.z);
				const float c2 = glm::cos(rotation.x);
				const float s2 = glm::sin(rotation.x);
				const float c1 = glm::cos(rotation.y);
				const float s1 = glm::sin(rotation.y);
				return glm::mat4{
					{
						scale.x * (c1 * c3 + s1 * s2 * s3),
						scale.x * (c2 * s3),
						scale.x * (c1 * s2 * s3 - c3 * s1),
						0.0f,
					},
					{
						scale.y * (c3 * s1 * s2 - c1 * s3),
						scale.y * (c2 * c3),
						scale.y * (c1 * c3 * s2 + s1 * s3),
						0.0f,
					},
					{
						scale.z * (c2 * s1),
						scale.z * (-s2),
						scale.z * (c1 * c2),
						0.0f,
					},
					{position.x, position.y, position.z, 1.0f}
				};
			}

	};
}

