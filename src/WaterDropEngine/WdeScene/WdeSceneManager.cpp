#include "WdeSceneManager.hpp"

namespace wde::scene {
	// Core functions
	void WdeSceneManager::initialize() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Initializing Scene Manager ==", LoggerChannel::SCENE_MANAGER);
		// Nothing to do here
		Logger::debug("== Initialization done ==", LoggerChannel::SCENE_MANAGER);
	}

	void WdeSceneManager::tick() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Updating scene.", LoggerChannel::SCENE_MANAGER);
		if (_scene != nullptr)
			_scene->update();

		Logger::debug("Rendering scene.", LoggerChannel::SCENE_MANAGER);
		if (_scene != nullptr)
			_scene->render();
	}

	void WdeSceneManager::cleanUp() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Cleaning up Scene Manager ==", LoggerChannel::SCENE_MANAGER);
		if (_scene != nullptr)
			_scene->cleanUp();
		Logger::debug("== Cleaning up done ==", LoggerChannel::SCENE_MANAGER);
	}


	// Setters and getters
	void WdeSceneManager::setScene(std::unique_ptr<Scene> scene) {
		WDE_PROFILE_FUNCTION();
		// Set scene
		WdeSceneManager::get()._scene = std::move(scene);
		// Initialize scene
		WdeSceneManager::get()._scene->initialize();
	}
}
