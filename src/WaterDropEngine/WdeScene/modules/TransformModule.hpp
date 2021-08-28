#pragma once

#include "../utils/SceneSerializerUtils.hpp"
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

			void renderGUI() override {
				gui::GUIRenderer::addVec3Button("Position", position);
				gui::GUIRenderer::addVec3Button("Rotation", rotation);
				gui::GUIRenderer::addVec3Button("Scale", scale, 1.0f);
			}


			// Serializers
			json serialize() override {
				return json({
					{"position", SceneSerializerUtils::asJson(position)},
					{"rotation", SceneSerializerUtils::asJson(rotation)},
					{"scale"   , SceneSerializerUtils::asJson(scale)}
				});
			}

			/** Deserialize the module */
			void deserialize(json data) override {
				position = SceneSerializerUtils::toVec3(data["position"]);
				rotation = SceneSerializerUtils::toVec3(data["rotation"]);
				scale    = SceneSerializerUtils::toVec3(data["scale"]);
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

			/** @return the normal game object matrix (= transpose(inverse(positionMatrix)) */
			glm::mat3 getNormalMatrix() const {
				const float c3 = glm::cos(rotation.z);
				const float s3 = glm::sin(rotation.z);
				const float c2 = glm::cos(rotation.x);
				const float s2 = glm::sin(rotation.x);
				const float c1 = glm::cos(rotation.y);
				const float s1 = glm::sin(rotation.y);
				const glm::vec3 invScale = 1.0f / scale;

				return glm::mat3 {
					{
						invScale.x * (c1 * c3 + s1 * s2 * s3),
						invScale.x * (c2 * s3),
						invScale.x * (c1 * s2 * s3 - c3 * s1)
					},
					{
						invScale.y * (c3 * s1 * s2 - c1 * s3),
						invScale.y * (c2 * c3),
						invScale.y * (c1 * c3 * s2 + s1 * s3)
					},
					{
						invScale.z * (c2 * s1),
						invScale.z * (-s2),
						invScale.z * (c1 * c2)
					}
				};
			}

	};
}

