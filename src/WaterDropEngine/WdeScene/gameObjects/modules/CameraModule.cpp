#include "CameraModule.hpp"
#include "../../../WaterDropEngine.hpp"

namespace wde::scene {
	CameraModule::CameraModule(GameObject &gameObject) : Module(gameObject, "Camera") {
		// Setup initial projection type
		auto aspect = WaterDropEngine::get().getRender().getInstance().getSwapchain().getAspectRatio();
		if (_projectionType == 0)
			setOrthographicProjection(aspect * _bottomCorner.x, aspect * _topCorner.x, _bottomCorner.y, _topCorner.y, _bottomCorner.z, _topCorner.z);
		else
			setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
	}

	void CameraModule::tick() {
		WDE_PROFILE_FUNCTION();
		// Update camera object based on it's game object transform associated position and rotation
		setViewYXZ(_gameObject.getModule<TransformModule>()->position, _gameObject.getModule<TransformModule>()->rotation);
		// setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f)); // Camera look to the right
		// setViewTarget(camera.getModule<TransformModule>().position, glm::vec3(0.0f, 0.0f, 0.0f)); // Look at center


		// Update projection type
		auto aspect = WaterDropEngine::get().getRender().getInstance().getSwapchain().getAspectRatio();
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

	void CameraModule::drawGUI() {
		// == Is selected camera ==
		auto actCamera = WaterDropEngine::get().getInstance().getScene().getActiveCamera();
		if (actCamera != nullptr && actCamera->getID() == _gameObject.getID())
			ImGui::Text("  Currently selected camera.");
		else if (ImGui::Button("Set as active"))
			setAsActive();
		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

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
			gui::GUIRenderer::addVec3Button("Bottom", _bottomCorner, -1.0f);
			gui::GUIRenderer::addVec3Button("Top", _topCorner, 1.0f);
			ImGui::TreePop();
		}

		// Perspective configuration
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		if (ImGui::TreeNode("Perspective")) {
			ImGui::Dummy(ImVec2(0.0f, 0.12f));
			gui::GUIRenderer::addFloatDragger("FOV", _fov, 60.0f);
			gui::GUIRenderer::addFloatDragger("Near Plane", _nearPlane, 0.1f);
			gui::GUIRenderer::addFloatDragger("Far Plane", _farPlane, 10.0f);
			ImGui::TreePop();
		}
		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));


		// == Culling ==
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("  Culling configuration");
		ImGui::Dummy(ImVec2(0.0f, 1.0f));
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		gui::GUIRenderer::addFloatDragger("Culling Distance", _cullingDistance, 100.0f);
	}

	void CameraModule::drawGizmo(Gizmo& gizmo, render::CommandBuffer& commandBuffer) {
		// Active camera, do not draw gizmo
		if (WaterDropEngine::get().getInstance().getScene()._activeCameraID == static_cast<int>(_gameObject.getID()))
			return;

		// Draw camera frustum gizmo
		auto t = _gameObject.transform->getTransform();
		auto thetaDiv2 = std::tan(glm::radians(_fov) / 2.0f);
		auto wNear = _aspect * _nearPlane * thetaDiv2;
		auto wFar  = _aspect * _farPlane  * thetaDiv2;

		// Draw cube
		gizmo.setColor(Color::GREEN);
		gizmo.drawCube(_gameObject.transform->position, _gameObject.transform->rotation, _gameObject.transform->scale);

		// Compute points
		auto topNearLeft  = t * glm::vec4 {-wNear, _nearPlane * thetaDiv2, _nearPlane, 1};
		auto topNearRight = t * glm::vec4 { wNear, _nearPlane * thetaDiv2, _nearPlane, 1};
		auto topFarLeft   = t * glm::vec4 {-wFar, _farPlane * thetaDiv2, _farPlane, 1};
		auto topFarRight  = t * glm::vec4 { wFar, _farPlane * thetaDiv2, _farPlane, 1};

		auto bottomNearLeft  = t * glm::vec4 {-wNear, -_nearPlane * thetaDiv2, _nearPlane, 1};
		auto bottomNearRight = t * glm::vec4 { wNear, -_nearPlane * thetaDiv2, _nearPlane, 1};
		auto bottomFarLeft   = t * glm::vec4 {-wFar, -_farPlane * thetaDiv2, _farPlane, 1};
		auto bottomFarRight  = t * glm::vec4 { wFar, -_farPlane * thetaDiv2, _farPlane, 1};

		gizmo.linesManager(Color::GREEN)
			// Between planes lines
			->addLine(topNearLeft, topFarLeft)
			->addLine(topNearRight, topFarRight)
			->addLine(bottomNearLeft , bottomFarLeft)
			->addLine(bottomNearRight, bottomFarRight)

			// Near plane
			->addLine(bottomNearLeft, bottomNearRight)
			->addLine(bottomNearRight, topNearRight)
			->addLine(topNearRight, topNearLeft)
			->addLine(topNearLeft, bottomNearLeft)

			// Far plane
			->addLine(bottomFarLeft, bottomFarRight)
			->addLine(bottomFarRight, topFarRight)
			->addLine(topFarRight, topFarLeft)
			->addLine(topFarLeft, bottomFarLeft)
		->drawLines(commandBuffer);
	}


	void CameraModule::setOrthographicProjection(float leftVal, float rightVal, float topVal, float bottomVal, float nearVal, float farVal)  {
		// Update class values
		auto aspect = WaterDropEngine::get().getRender().getInstance().getSwapchain().getAspectRatio();
		_aspect = aspect;
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

	void CameraModule::setAsActive() {
		WaterDropEngine::get().getInstance().getScene()._activeCameraID = static_cast<int>(_gameObject.getID());
	}

	void CameraModule::setFarPlane(float farPlane) {
		_farPlane = farPlane;
		auto aspect = WaterDropEngine::get().getRender().getInstance().getSwapchain().getAspectRatio();
		setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
	}
}
