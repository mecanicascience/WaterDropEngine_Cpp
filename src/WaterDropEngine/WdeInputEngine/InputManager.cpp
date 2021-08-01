#include "InputManager.hpp"

namespace wde::inputManager {
	// Core functions
	void InputManager::initialize() {
		Logger::debug("== Initializing Input manager ==", LoggerChannel::INPUT_MANAGER);
		Logger::debug("Setting default keymap configuration.", LoggerChannel::INPUT_MANAGER);
		setDefaultKeyMapping();

		// Logger::debug("Setting user keymap configuration.", LoggerChannel::INPUT_MANAGER);
		// setUserKeyMapping(); // TODO : Load keymap from user preferences file
		Logger::debug("== Initialization Done ==", LoggerChannel::INPUT_MANAGER);
	}

	void InputManager::tick() { }

	void InputManager::cleanUp() {
		Logger::debug("== Cleaning up Input manager ==", LoggerChannel::INPUT_MANAGER);
		// Logger::debug("Saving user keymap configuration.", LoggerChannel::INPUT_MANAGER);
		// saveUserKeyMapping(); // TODO : Save user new keymap

		Logger::debug("== Cleaning up Done ==", LoggerChannel::INPUT_MANAGER);
	}



	// Helpers
	void InputManager::setDefaultKeyMapping() {
		// TODO : Load from file too
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
			throw WdeException("The default key mapping misses " + std::to_string(glm::abs(KEY_COUNT - _userKeyMapping.size())) + " key binding(s).", LoggerChannel::INPUT_MANAGER);
	}


	// Getters and setters
	bool InputManager::isKeyDown(const std::string& keyName) {
		auto glfwWindow = renderEngine::WdeRenderEngine::get().getWindow();
		return glfwGetKey(glfwWindow, _userKeyMapping.at(keyName)) == GLFW_PRESS;
	}
}
