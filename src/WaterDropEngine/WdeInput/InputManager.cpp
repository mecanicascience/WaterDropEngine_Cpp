#include "InputManager.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::input {
	InputManager::InputManager() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::INPUT) << "== Initializing Input manager ==" << logger::endl;
		logger::log(LogLevel::DEBUG, LogChannel::INPUT) << "Setting default keymap configuration." << logger::endl;
		setDefaultKeyMapping();
		logger::log(LogLevel::DEBUG, LogChannel::INPUT) << "== Initialization Done ==" << logger::endl;
	}

	InputManager::~InputManager() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::INPUT) << "== Cleaning up Input manager ==" << logger::endl;
		// Nothing to do
		logger::log(LogLevel::DEBUG, LogChannel::INPUT) << "== Cleaning up Done ==" << logger::endl;
	}




	void InputManager::setDefaultKeyMapping() {
		WDE_PROFILE_FUNCTION();
		// For now, set manually every required keys
		addKey("moveForward", GLFW_KEY_W);
		addKey("moveBackward", GLFW_KEY_S);
		addKey("moveRight", GLFW_KEY_D);
		addKey("moveLeft", GLFW_KEY_A);
		addKey("moveUp", GLFW_KEY_E);
		addKey("moveDown", GLFW_KEY_Q);
		addKey("lookUp", GLFW_KEY_UP);
		addKey("lookDown", GLFW_KEY_DOWN);
		addKey("lookRight", GLFW_KEY_RIGHT);
		addKey("lookLeft", GLFW_KEY_LEFT);

		// Check if number of keys is as expected
		if (_userKeyMapping.size() != KEY_COUNT)
			throw WdeException(LogChannel::INPUT, "The default key mapping misses " + std::to_string(glm::abs(KEY_COUNT - _userKeyMapping.size())) + " key binding(s).");
	}

	void InputManager::addKey(const std::string &keyName, int keyID) {
		if (_userKeyMapping.contains(keyName))
			throw WdeException(LogChannel::INPUT, "Two key strokes have the same name (" + keyName + ").");
		_userKeyMapping.emplace(keyName, keyID);
	}

	bool InputManager::isKeyDown(const std::string &keyName) const {
		return glfwGetKey(WaterDropEngine::get().getRender().getWindow().getWindow(), _userKeyMapping.at(keyName)) == GLFW_PRESS;
	}
}
