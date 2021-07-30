#pragma once

#include "../../../wde.hpp"

namespace wde::renderStructures {
	class Camera {
		public:
			// Core functions
			/**
			 * Camera will use an orthographic projection
			 * @param leftVal First bound x value
			 * @param rightVal Second bound x value
			 * @param topVal Second bound y value
			 * @param bottomVal First bound y value
			 * @param nearVal First bound z value
			 * @param farVal Second bound z value
			 */
			void setOrthographicProjection(float leftVal, float rightVal, float topVal, float bottomVal, float nearVal, float farVal);

			/**
			 * Camera will use a perspective projection
			 * @param fovY Vertical field of view
			 * @param aspect The width/height ratio of the near plane
			 * @param nearVal The near plane distance
			 * @param farVal The far plane distance
			 */
			void setPerspectiveProjection(float fovY, float aspect, float nearVal, float farVal);



			/**
			 * Set the camera viewing direction
			 * @param position Camera position
			 * @param direction Camera viewing direction vector
			 * @param up Camera up vector direction
			 */
			void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});

			/**
			 * Set the view camera target
			 * @param position Camera position
			 * @param target Camera viewing target position
			 * @param up Camera up vector direction
			 */
			void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});

			/**
			 * Set the view camera coordinates
			 * @param position Camera position
			 * @param rotation Camera axis rotation
			 */
			void setViewYXZ(glm::vec3 position, glm::vec3 rotation);



			// Getters and setters
			const glm::mat4& getProjection() const { return _projectionMatrix; }
			const glm::mat4& getView() const { return _viewMatrix; }


		private:
			glm::mat4 _projectionMatrix {1.0f};
			glm::mat4 _viewMatrix {1.0f};
	};
}
