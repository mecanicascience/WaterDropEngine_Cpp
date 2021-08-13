#include "CameraModule.hpp"
#include "../objects/GameObject.hpp"

namespace wde::scene {
	CameraModule::CameraModule(GameObject &gameObject) : Module(gameObject, "Camera") {}

	// Core functions
	void CameraModule::initialize() {
		// Setup initial projection type
		auto aspect = CoreInstance::get().getSelectedDevice().getSwapChain().getAspectRatio();
		if (_projectionType == 0)
			setOrthographicProjection(aspect * _bottomCorner.x, aspect * _topCorner.x, _bottomCorner.y, _topCorner.y, _bottomCorner.z, _topCorner.z);
		else
			setPerspectiveProjection(_fov, aspect, _nearPlane, _farPlane);
	}


	void CameraModule::update(float deltaTime) {
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


	void CameraModule::renderGUI() {
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
	}


	// Serializers
	json CameraModule::serialize() {
		return json({
			{"projectionMatrix", SceneSerializerUtils::asJson(_projectionMatrix)},
			{"viewMatrix"      , SceneSerializerUtils::asJson(_viewMatrix)},
			{"projectionType"  , _projectionType},
			{"orthographic", {
				{"bottomCorner", SceneSerializerUtils::asJson(_bottomCorner)},
				{"topCorner", SceneSerializerUtils::asJson(_topCorner)}
			}},
			{"perspective", {
				{"fov", _fov},
				{"nearPlane", _nearPlane},
				{"farPlane", _farPlane}
			}}
		});
	}

	void CameraModule::deserialize(json data) {
		// Matrices
		_projectionMatrix = SceneSerializerUtils::toMat4(data["projectionMatrix"]);
		_viewMatrix       = SceneSerializerUtils::toMat4(data["viewMatrix"]);
		_projectionType   = int(data["projectionType"]);

		// Orthographic
		_bottomCorner = SceneSerializerUtils::toVec3(data["orthographic"]["bottomCorner"]);
		_topCorner    = SceneSerializerUtils::toVec3(data["orthographic"]["topCorner"]);

		// Perspective
		_fov       = float(data["perspective"]["fov"]);
		_nearPlane = float(data["perspective"]["nearPlane"]);
		_farPlane  = float(data["perspective"]["farPlane"]);
	}
}

