#pragma once

#include "Module.hpp"
#include "../../WdeRenderingEngine/WdeRenderEngine.hpp"

namespace wde::scene {
	/**
	 * Handles a Camera
	 */
	class CameraModule : public Module {
		public:
			explicit CameraModule(GameObject &gameObject) : Module(gameObject, "Camera") {}

			// Core functions
			void initialize() override {
				// Setup initial projection type
				auto aspect = CoreInstance::get().getSelectedDevice().getSwapChain().getAspectRatio();
				if (_projectionType == 0)
					setOrthographicProjection(aspect * _bottomCorner.x, aspect * _topCorner.x, _bottomCorner.y, _topCorner.y, _bottomCorner.z, _topCorner.z);
				else
					setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
			}


			void update(float deltaTime) override {
				// Update camera object based on it's game object transform associated position and rotation
				setViewYXZ(_gameObject.getModule<TransformModule>().position, _gameObject.getModule<TransformModule>().rotation);
				// setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f)); // Camera look to the right
				// setViewTarget(camera.getModule<TransformModule>().position, glm::vec3(0.0f, 0.0f, 0.0f)); // Look at center


				// Update projection type
				auto aspect = CoreInstance::get().getSelectedDevice().getSwapChain().getAspectRatio();
				static int lastProjectionType = _projectionType;
				if (lastProjectionType != _projectionType) {
					lastProjectionType = _projectionType;
					if (_projectionType == 0)
						setOrthographicProjection(aspect * _bottomCorner.x, aspect * _topCorner.x, _bottomCorner.y, _topCorner.y, _bottomCorner.z, _topCorner.z);
					else
						setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
				}

				// Orthographic
				static glm::vec3 bottomCornerLast = _bottomCorner;
				if (_bottomCorner != bottomCornerLast) {
					bottomCornerLast = _bottomCorner;
					if (_projectionType == 0)
						setOrthographicProjection(aspect * _bottomCorner.x, aspect * _topCorner.x, _bottomCorner.y, _topCorner.y, _bottomCorner.z, _topCorner.z);
				}

				static glm::vec3 topCornerLast = _topCorner;
				if (_topCorner != topCornerLast) {
					topCornerLast = _topCorner;
					if (_projectionType == 0)
						setOrthographicProjection(aspect * _bottomCorner.x, aspect * _topCorner.x, _bottomCorner.y, _topCorner.y, _bottomCorner.z, _topCorner.z);
				}


				// Perspective
				static float fovLast = _fov;
				if (_fov != fovLast) {
					fovLast = _fov;
					if (_projectionType == 1)
						setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
				}

				static float nearPlaneLast = _nearPlane;
				if (_nearPlane != nearPlaneLast) {
					nearPlaneLast = _nearPlane;
					if (_projectionType == 1)
						setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
				}

				static float farPlaneLast = _farPlane;
				if (_farPlane != farPlaneLast) {
					farPlaneLast = _farPlane;
					if (_projectionType == 1)
						setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
				}
			}


			void renderGUI() override {
				// == Projection type GUI ==
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
				ImGui::Text("  Projection configuration");
				ImGui::Dummy(ImVec2(0.0f, 1.0f));
				ImGui::PopFont();
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 140.0f);

				// Selected projection
				ImGui::Text("Selected projection");

				ImGui::NextColumn();
				ImGui::PushID("projection_type_combo");
				ImGui::Combo("", &_projectionType, "Orthographic\0Perspective\0");
				ImGui::PopID();
				ImGui::Columns(1);

				// Orthographic configuration
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				if (ImGui::TreeNode("Orthographic")) {
					ImGui::Dummy(ImVec2(0.0f, 0.12f));
					gui::GUIRenderer::createVec3Button("Bottom", _bottomCorner, -1.0f);
					gui::GUIRenderer::createVec3Button("Top", _topCorner, 1.0f);
					ImGui::TreePop();
				}

				// Perspective configuration
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				if (ImGui::TreeNode("Perspective")) {
					ImGui::Dummy(ImVec2(0.0f, 0.12f));
					gui::GUIRenderer::addFloatDragger("FOV", _fov, glm::radians(60.0f));
					gui::GUIRenderer::addFloatDragger("Near Plane", _nearPlane, 0.1f);
					gui::GUIRenderer::addFloatDragger("Far Plane", _farPlane, 10.0f);
					ImGui::TreePop();
				}
			}


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
			float _farPlane = 10.0f;
	};
}

