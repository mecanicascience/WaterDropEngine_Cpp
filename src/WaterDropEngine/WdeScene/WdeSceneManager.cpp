#include "WdeSceneManager.hpp"
#include "../WdeGUI/WdeGUI.hpp"

namespace wde::scene {
	// Core functions
	void WdeSceneManager::initialize() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Initializing Scene Manager ==", LoggerChannel::SCENE);
		Logger::debug("Initializing selected scene.", LoggerChannel::SCENE);
		if (_scene == nullptr)
			throw WdeException("Scene not set before initialization.", LoggerChannel::SCENE);
		_scene->initialize();

		_hasStarted = true;
		Logger::debug("== Initialization done ==", LoggerChannel::SCENE);
	}

	void WdeSceneManager::tick() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Updating scene game objects.", LoggerChannel::SCENE);
		if (_scene != nullptr)
			_scene->update();
	}

	void WdeSceneManager::cleanUp() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Cleaning up Scene ==", LoggerChannel::SCENE);
		if (_scene != nullptr)
			_scene->cleanUp();
		Logger::debug("== Cleaning up done ==", LoggerChannel::SCENE);
	}


	// GUI functions
	void WdeSceneManager::setupGUI(ImGuiID &parentID) {
		Logger::debug("Setting up scene GUI.", LoggerChannel::SCENE);
		if (_scene != nullptr)
			_scene->setupGUI(parentID);
	}

	void WdeSceneManager::renderGUI() {
		Logger::debug("Starting scene GUI rendering.", LoggerChannel::SCENE);
		if (_scene != nullptr)
			_scene->renderGUI();
	}



	// Setters and getters
	void WdeSceneManager::setScene(std::unique_ptr<Scene> scene) {
		WDE_PROFILE_FUNCTION();
		// Delete last scene
		if (WdeSceneManager::get()._scene != nullptr)
			WdeSceneManager::get()._scene->cleanUp();

		// Set new scene
		WdeSceneManager::get()._scene = std::move(scene);

		// Initialize scene if the scene manager has already started
		if (WdeSceneManager::get()._hasStarted)
			WdeSceneManager::get()._scene->initialize();

		// Say that GUI scene should be reset on next render frame
		gui::WdeGUI::get().reset();
	}
}
