#pragma once

#include "Module.hpp"
#include "../../WdeRenderingEngine/WdeRenderEngine.hpp"
#include "../utils/SceneSerializerUtils.hpp"

namespace wde::scene {
	/**
	 * Handles a Camera
	 */
	class CameraModule : public Module {
		public:
			/**
			 * Matrix that describes camera projection matrices
			 */
			struct GPUCameraData {
				alignas(16) glm::mat4 transformCameraSpace {1.0f};
				alignas(16) glm::mat4 transformProjSpace   {1.0f};
				float nearPlane {0.0f};
				float farPlane {1.0f};
			};


			explicit CameraModule(GameObject &gameObject);

			// Core functions
			void initialize() override;
			void update(float deltaTime) override;
			void renderGUI() override;


			GPUCameraData getCameraData() const {
				GPUCameraData data {};
				data.transformCameraSpace = getView();
				data.transformProjSpace   = getProjection();
				data.nearPlane = _nearPlane;
				data.farPlane  = _farPlane;
				return data;
			}


			// Serializers
			json serialize() override;
			void deserialize(json data) override;




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
			void setOrthographicProjection(float leftVal, float rightVal, float topVal, float bottomVal, float nearVal, float farVal) {
				// Update class values
				auto aspect = CoreInstance::get().getSelectedDevice().getSwapChain().getAspectRatio();
				_bottomCorner = {leftVal / aspect, topVal, nearVal};
				_topCorner = {rightVal / aspect, bottomVal, farVal};

				_projectionMatrix = glm::mat4{1.0f};
				_projectionMatrix[0][0] =  2.0f / (rightVal - leftVal);
				_projectionMatrix[1][1] =  2.0f / (bottomVal - topVal);
				_projectionMatrix[2][2] =  1.0f / (farVal - nearVal);
				_projectionMatrix[3][0] = -(rightVal + leftVal) / (rightVal - leftVal);
				_projectionMatrix[3][1] = -(bottomVal + topVal) / (bottomVal - topVal);
				_projectionMatrix[3][2] = -nearVal / (farVal - nearVal);
			}

			/**
			 * Camera will use a perspective projection
			 * @param fovY Vertical field of view (in degrees)
			 * @param aspect The width/height ratio of the near plane
			 * @param nearVal The near plane distance
			 * @param farVal The far plane distance
			 */
			void setPerspectiveProjection(float fovY, float aspect, float nearVal, float farVal) {
				if (glm::abs(aspect - std::numeric_limits<float>::epsilon()) <= 0.0f)
					throw WdeException("The camera aspect ratio is too small.", LoggerChannel::SCENE);

				// Update class values
				_nearPlane = nearVal;
				_farPlane = farVal;
				_fov = fovY;

				const float tanHalfFovy = std::tan(glm::radians(fovY) / 2.0f);
				_projectionMatrix = glm::mat4{0.0f};
				_projectionMatrix[0][0] =  1.0f / (aspect * tanHalfFovy);
				_projectionMatrix[1][1] =  1.0f / (tanHalfFovy);
				_projectionMatrix[2][2] =  farVal / (farVal - nearVal);
				_projectionMatrix[2][3] =  1.0f;
				_projectionMatrix[3][2] = -(farVal * nearVal) / (farVal - nearVal);
			}



			/**
			 * Set the camera viewing direction
			 * @param position Camera position
			 * @param direction Camera viewing direction vector
			 * @param up Camera up vector direction
			 */
			void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f}) {
				const glm::vec3 w{glm::normalize(direction)};
				const glm::vec3 u{glm::normalize(glm::cross(w, up))};
				const glm::vec3 v{glm::cross(w, u)};

				_viewMatrix = glm::mat4{1.f};
				_viewMatrix[0][0] = u.x;
				_viewMatrix[1][0] = u.y;
				_viewMatrix[2][0] = u.z;
				_viewMatrix[0][1] = v.x;
				_viewMatrix[1][1] = v.y;
				_viewMatrix[2][1] = v.z;
				_viewMatrix[0][2] = w.x;
				_viewMatrix[1][2] = w.y;
				_viewMatrix[2][2] = w.z;
				_viewMatrix[3][0] = -glm::dot(u, position);
				_viewMatrix[3][1] = -glm::dot(v, position);
				_viewMatrix[3][2] = -glm::dot(w, position);
			}

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
			void setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
				const float c3 = glm::cos(rotation.z);
				const float s3 = glm::sin(rotation.z);
				const float c2 = glm::cos(rotation.x);
				const float s2 = glm::sin(rotation.x);
				const float c1 = glm::cos(rotation.y);
				const float s1 = glm::sin(rotation.y);
				const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
				const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
				const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
				_viewMatrix = glm::mat4{1.f};
				_viewMatrix[0][0] = u.x;
				_viewMatrix[1][0] = u.y;
				_viewMatrix[2][0] = u.z;
				_viewMatrix[0][1] = v.x;
				_viewMatrix[1][1] = v.y;
				_viewMatrix[2][1] = v.z;
				_viewMatrix[0][2] = w.x;
				_viewMatrix[1][2] = w.y;
				_viewMatrix[2][2] = w.z;
				_viewMatrix[3][0] = -glm::dot(u, position);
				_viewMatrix[3][1] = -glm::dot(v, position);
				_viewMatrix[3][2] = -glm::dot(w, position);
			}

			// Getters and setters
			const glm::mat4& getProjection() const { return _projectionMatrix; }
			const glm::mat4& getView() const { return _viewMatrix; }


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
			float _farPlane = 100.0f;
	};
}

