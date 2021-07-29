#pragma once

#include "../../../wde.hpp"
#include "Model.hpp"

namespace wde::renderEngine {
	struct Transform2DComponent {
		glm::vec2 translation {0.0f, 0.0f};
		glm::vec2 scale {1.0f, 1.0f};
		float rotation = 0.0f;

		// Return the corresponding transform matrix
		glm::mat2 mat2() {
			const float sinRot = glm::sin(rotation);
			const float cosRot = glm::cos(rotation);
			glm::mat2 rotMatrix {{cosRot, sinRot}, {-sinRot, cosRot}};

			glm::mat2 scaleMat {{scale.x, 0.0f}, {0.0f, scale.y}};

			return rotMatrix * scaleMat;
		}
	};

	class GameObject {
		public:
			using id_t = unsigned int;
			std::shared_ptr<Model> model;
			Transform2DComponent transform2D;

			// Constructors
			static GameObject createGameObject() {
				static id_t currentID = 0;
				return GameObject(currentID++);
			}


			// Getters and setters
			id_t getID() const { return _objectID; }


			// Core functions
			/**
			 * Draws the game object to the screen
			 * @param commandBuffer
			 */
			void render(CommandBuffer &commandBuffer) {
				model->bind(commandBuffer);
				model->render();
			}


		private:
			id_t _objectID;

			/**
			 * Creates a new game object
			 * @param objectID
			 */
			explicit GameObject(id_t objectID) : _objectID(objectID) {}
	};
}

