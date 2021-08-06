#include "WdeSceneManager.hpp"

namespace wde::scene {
	// Core functions
	void WdeSceneManager::initialize() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Initializing Scene Manager ==", LoggerChannel::SCENE_MANAGER);
		Logger::debug("Initializing selected scene.", LoggerChannel::SCENE_MANAGER);
		if (_scene == nullptr)
			throw WdeException("Scene not set before initialization.", LoggerChannel::SCENE_MANAGER);
		_scene->initialize();

		_hasStarted = true;
		Logger::debug("== Initialization done ==", LoggerChannel::SCENE_MANAGER);
	}

	void WdeSceneManager::tick() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Updating scene game objects.", LoggerChannel::SCENE_MANAGER);
		if (_scene != nullptr)
			_scene->update();
	}

	void WdeSceneManager::cleanUp() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Cleaning up Scene ==", LoggerChannel::SCENE_MANAGER);
		if (_scene != nullptr)
			_scene->cleanUp();
		Logger::debug("== Cleaning up done ==", LoggerChannel::SCENE_MANAGER);
	}


	// Setters and getters
	void WdeSceneManager::setScene(std::unique_ptr<Scene> scene) {
		WDE_PROFILE_FUNCTION();
		// Set scene
		WdeSceneManager::get()._scene = std::move(scene);

		if (WdeSceneManager::get()._hasStarted) // Initialize scene if the scene manager has already started
			WdeSceneManager::get()._scene->initialize();
	}
}
