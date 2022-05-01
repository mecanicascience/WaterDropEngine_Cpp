#pragma once

#include "Module.hpp"

namespace wde::scene {
	/**
	 * Describes a camera game object
	 */
	class CameraModule : public Module {
		public:
			explicit CameraModule(GameObject& gameObject);
			explicit CameraModule(GameObject& gameObject, const std::string& data);

			void tick() override;
			void drawGUI() override;
			void drawGizmo(Gizmo& gizmo, render::CommandBuffer& commandBuffer) override;
			json serialize() override;

			/** Sets this camera to be the current scene viewing camera */
			void setAsActive();



			// Camera core functions
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
			 * @param fovY Vertical field of view (in degrees)
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
			void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f}) {
				setViewDirection(position, target - position, up);
			}

			/**
			 * Set the view camera coordinates
			 * @param position Camera position
			 * @param rotation Camera axis rotation
			 */
			void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

			// Getters and setters
			const glm::mat4& getProjection() const { return _projectionMatrix; }
			const glm::mat4& getView() const { return _viewMatrix; }
			float getNear() const { return _nearPlane; }
			float getFar() const { return _farPlane; }
			float getAspect() const { return _aspect; }
			float getFov() const { return _fov; }
			void setFarPlane(float farPlane);



		private:
			/** Projection of object coordinates to Vulkan coordinates */
			glm::mat4 _projectionMatrix {1.0f};

			/** Projection of camera position and rotation */
			glm::mat4 _viewMatrix {1.0f};


			// Core configuration
			// Type of projection used
			int _projectionType = 1; // 0 : Orthographic - 1 : Perspective

			// Orthographic
			glm::vec3 _bottomCorner {-1, -1, -1};
			glm::vec3 _topCorner {1, 1, 1};

			// Perspective
			float _fov = 60.0f;
			float _nearPlane = 0.1f;
			float _farPlane = 500.0f;
			float _aspect = 4.0 / 3.0;
	};
}
