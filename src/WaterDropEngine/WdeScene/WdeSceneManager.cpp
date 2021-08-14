#include "WdeSceneManager.hpp"
#include "../WdeGUI/WdeGUI.hpp"

namespace wde::scene {
	// Constructors
	WdeSceneManager::~WdeSceneManager() {
		_initializeSceneThread.join();
	}



	// Core functions
	void WdeSceneManager::initialize() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Initializing Scene Manager ==", LoggerChannel::SCENE);
		if (_scene == nullptr) {
			Logger::warn("Scene not set before initialization.", LoggerChannel::SCENE);
			_initializeSceneThread.join();
			return;
		}

		// Initialize scene
		Logger::info("Initializing scene asynchronously.", LoggerChannel::SCENE);
		_initializeSceneThread = std::thread([this]{
			// Initialize scene components
			Logger::debug("Initializing selected scene.", LoggerChannel::SCENE);
			_scene->initialize();

			// Initialize scene game objects
			Logger::debug("Initializing selected scene game objects.", LoggerChannel::SCENE);
			_scene->initializeGameObjects();

			// Say that the scene is initialized
			Logger::info("Scene loaded.", LoggerChannel::SCENE);
			_scene->setInitialized();
		});

		// Start manager
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
		WDE_PROFILE_FUNCTION();
		Logger::debug("Setting up scene GUI.", LoggerChannel::SCENE);
		if (_scene != nullptr)
			_scene->setupGUI(parentID);
	}

	void WdeSceneManager::renderGUI() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Starting scene GUI rendering.", LoggerChannel::SCENE);
		if (_scene != nullptr)
			_scene->renderGUI();
	}



	// Serializers
	void WdeSceneManager::serializeScene() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Serializing scene.", LoggerChannel::SCENE);

		if (_scene == nullptr) {
			Logger::warn("Cannot serialize an empty scene.", LoggerChannel::SCENE);
			return;
		}

		// Serialize scene
		json sceneJSON = _scene->serialize();
		std::string sceneName = _scene->getName();
		std::transform(sceneName.begin(), sceneName.end(), sceneName.begin(), [](unsigned char c){ return std::tolower(c); });
		for (char& character : sceneName)
			if (character == ' ')
				character = '_';
		WdeFileUtils::saveFileDialog(to_string(sceneJSON), "json");
		Logger::info("Scene saved.", LoggerChannel::SCENE);
	}

	void WdeSceneManager::deserializeScene(const json& sceneJSONContent) {
		WDE_PROFILE_FUNCTION();
		// Set scene
		setScene(std::make_unique<LoadedScene>(sceneJSONContent));

		// Deserialize scene
		Logger::info("Loading scene asynchronously.", LoggerChannel::SCENE);
		_initializeSceneThread.join();
		_initializeSceneThread = std::thread([this, sceneJSONContent]{
			WDE_PROFILE_FUNCTION();
			// Initialize scene components
			Logger::debug("Initializing selected scene.", LoggerChannel::SCENE);
			_scene->initialize();

			// Initialize scene game objects
			Logger::debug("Initializing selected scene game objects.", LoggerChannel::SCENE);
			_scene->initializeGameObjects();

			// Deserialize scene
			Logger::debug("Deserializing scene.", LoggerChannel::SCENE);
			_scene->deserialize(sceneJSONContent);

			// Say that the scene is initialized
			Logger::info("Scene loaded.", LoggerChannel::SCENE);
			_scene->setInitialized();
		});
	}



	// Setters and getters
	void WdeSceneManager::setScene(std::unique_ptr<Scene> scene) {
		WDE_PROFILE_FUNCTION();

		// Delete last scene
		if (WdeSceneManager::get()._scene != nullptr) {
			Logger::debug("Cleaning up last scene", LoggerChannel::SCENE);
			WdeRenderEngine::get().waitForDevicesReady();
			WdeSceneManager::get()._scene->cleanUp();
			WdeSceneManager::get()._scene.reset();
		}

		// Set new scene
		WdeSceneManager::get()._scene = std::move(scene);

		// Say that GUI scene should be reset on next render frame
		gui::WdeGUI::get().reset();
	}
}
