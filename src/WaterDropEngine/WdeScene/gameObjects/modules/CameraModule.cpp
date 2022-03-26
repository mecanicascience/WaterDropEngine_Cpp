#include "CameraModule.hpp"
#include "../../../WaterDropEngine.hpp"

namespace wde::scene {
	CameraModule::CameraModule(GameObject &gameObject) : Module(gameObject, "Camera", ICON_FA_CAMERA) {
		// Setup initial projection type
		auto aspect = WaterDropEngine::get().getRender().getInstance().getSwapchain().getAspectRatio();
		if (_projectionType == 0)
			setOrthographicProjection(aspect * _bottomCorner.x, aspect * _topCorner.x, _bottomCorner.y, _topCorner.y, _bottomCorner.z, _topCorner.z);
		else
			setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
	}

	CameraModule::CameraModule(GameObject &gameObject, const std::string& data) : Module(gameObject, "Camera", ICON_FA_CAMERA) {
		auto dataJ = json::parse(data);
		// Set data
		_projectionType = dataJ["projectionType"].get<int>();
		_bottomCorner = glm::vec3 {
			dataJ["orthographic"]["bottom"][0].get<float>(),
			dataJ["orthographic"]["bottom"][1].get<float>(),
			dataJ["orthographic"]["bottom"][2].get<float>()
		};
		_topCorner = glm::vec3 {
				dataJ["orthographic"]["top"][0].get<float>(),
				dataJ["orthographic"]["top"][1].get<float>(),
				dataJ["orthographic"]["top"][2].get<float>()
		};
		_fov = dataJ["perspective"]["fov"].get<float>();
		_nearPlane = dataJ["perspective"]["nearPlane"].get<float>();
		_farPlane = dataJ["perspective"]["farPlane"].get<float>();

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
		// Selected projection
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 140.0f);
		ImGui::Dummy(ImVec2(0.0, 1.0f));
		ImGui::Text("Selected projection");

		ImGui::NextColumn();
		ImGui::PushID("projection_type_combo");
		ImGui::Combo("", &_projectionType, "Orthographic\0Perspective\0");
		ImGui::PopID();
		ImGui::Columns(1);
		ImGui::Separator();

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



	// Views
	void CameraModule::setPerspectiveProjection(float fovY, float aspect, float nearVal, float farVal) {
		if (glm::abs(aspect - std::numeric_limits<float>::epsilon()) <= 0.0f)
			throw WdeException(LogChannel::SCENE, "The camera aspect ratio is too small.");

		// Update class values
		_nearPlane = nearVal;
		_farPlane = farVal;
		_fov = fovY;
		_aspect = aspect;

		const float tanHalfFovy = std::tan(glm::radians(fovY) / 2.0f);
		_projectionMatrix = glm::mat4{0.0f};
		_projectionMatrix[0][0] =  1.0f / (aspect * tanHalfFovy);
		_projectionMatrix[1][1] =  1.0f / (tanHalfFovy);
		_projectionMatrix[2][2] =  farVal / (farVal - nearVal);
		_projectionMatrix[2][3] =  1.0f;
		_projectionMatrix[3][2] = -(farVal * nearVal) / (farVal - nearVal);
	}

	void CameraModule::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
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

	void CameraModule::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
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
}
