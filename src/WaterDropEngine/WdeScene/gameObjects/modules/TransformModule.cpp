#include "TransformModule.hpp"
#include "../GameObject.hpp"

namespace wde::scene {
	TransformModule::TransformModule(GameObject &gameObject) : Module(gameObject, "Transform") {}

	TransformModule::~TransformModule() {
		_parent = nullptr;
		_childrenIDs.clear();
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
}

