#include "TransformModule.hpp"
#include "../GameObject.hpp"

namespace wde::scene {
	TransformModule::TransformModule(GameObject &gameObject) : Module(gameObject, "Transform", ICON_FA_GLOBE) {}

	TransformModule::~TransformModule() {
		_parent = nullptr;
		_childrenIDs.clear();
	}

	void TransformModule::tick() {
		WDE_PROFILE_FUNCTION();
		auto newTransform = getTransform();
		if (_lastTransform != newTransform) {
			changed = true;
			_lastTransform = newTransform;
		}
	}


	void TransformModule::setParent(TransformModule *parent) {
		// Remove last parent children ID
		if (_parent != nullptr)
			_parent->_childrenIDs.erase(std::remove(_parent->_childrenIDs.begin(), _parent->_childrenIDs.end(), _gameObject.getID()), _parent->_childrenIDs.end());
		// Change parent
		_parent = parent;
		// Add children to new parent
		_parent->_childrenIDs.push_back(static_cast<int>(_gameObject.getID()));
	}

	void TransformModule::drawGUI() {
		gui::GUIRenderer::addVec3Button("Position", position);
		gui::GUIRenderer::addVec3Button("Rotation", rotation);
		gui::GUIRenderer::addVec3Button("Scale", scale, 1.0f);
	}

	glm::mat4 TransformModule::getTransform() const {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		auto mat = glm::mat4{
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
		if (_parent != nullptr && _parent != this)
			mat = _parent->getTransform() * mat;
		return mat;
	}
}

