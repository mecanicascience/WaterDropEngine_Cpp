#include "WdeSceneManager.hpp"
#include "../WdeGUI/WdeGUI.hpp"
#include "scene/LoadedScene.hpp"
#include "scene/EmptyScene.hpp"

namespace wde::scene {
	// Constructors
	WdeSceneManager::~WdeSceneManager() {
	    if (_initializeSceneThread.joinable())
		    _initializeSceneThread.join();
	}


	// Core functions
	void WdeSceneManager::initialize() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Initializing Scene Manager ==", LoggerChannel::SCENE);
		if (_scene != nullptr)
			throw WdeException("Scene must be set after initialization.", LoggerChannel::SCENE);

		// Set waiting scene (synchronously)
		Logger::debug("Initializing waiting scene.", LoggerChannel::SCENE);
		WdeSceneManager::get()._scene = std::move(std::make_unique<EmptyScene>());
		_scene->initialize();
		_scene->initializeGameObjects();
		_scene->setInitialized();
		gui::WdeGUI::get().reset();

		// Start manager
		Logger::debug("== Initialization done ==", LoggerChannel::SCENE);
	}

	static bool isFirstTick = true;
	void WdeSceneManager::tick() {
		WDE_PROFILE_FUNCTION();
		if (isFirstTick) { // Do not start scene on first tick
			isFirstTick = false;
			return;
		}

		if (_sceneToAdd != nullptr) { // Should add a new scene
			// Delete last scene
			if (_scene != nullptr) {
				Logger::debug("Cleaning up last scene", LoggerChannel::SCENE);
				// Wait for device
				WdeRenderEngine::get().waitForDevicesReady();

				// Clear scene
				_scene->cleanUp();
				_scene.reset();
			}

			// Set new scene (will be initialized on the next tick of the scene manager)
			_scene = std::move(_sceneToAdd);
			_sceneToAdd = nullptr;

			// Say that GUI scene should be reset on next render frame
			gui::WdeGUI::get().reset();

			// Initialize scene async
			if (_loadAsync) {
                Logger::info("Initializing scene asynchronously.", LoggerChannel::SCENE);
                if (_initializeSceneThread.joinable())
                    _initializeSceneThread.join();
                _initializeSceneThread = std::thread([this]{
                    setupScene();
                });
			}
			else {
			    Logger::info("Initializing scene.", LoggerChannel::SCENE);
			    setupScene();
            }


			return;
		}

		if (_scene != nullptr) {
			Logger::debug("Updating scene game objects.", LoggerChannel::SCENE);
			_scene->update();
		}
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
		if (_loadAsync) {
            Logger::info("Loading scene asynchronously.", LoggerChannel::SCENE);
            _initializeSceneThread.join();
            _initializeSceneThread = std::thread([this, sceneJSONContent]{
                setupScene(sceneJSONContent);
            });
		}
		else {
		    Logger::info("Loading scene.", LoggerChannel::SCENE);
		    setupScene(sceneJSONContent);
		}
	}



	// Setup scene configuration
	void WdeSceneManager::setupScene(json sceneJSONContent) {
        WDE_PROFILE_FUNCTION();
        // Initialize scene components
        Logger::debug("Initializing selected scene.", LoggerChannel::SCENE);
        _scene->initialize();

        // Initialize scene game objects
        Logger::debug("Initializing selected scene game objects.", LoggerChannel::SCENE);
        _scene->initializeGameObjects();

        // Deserialize scene
        if (!sceneJSONContent.empty()) {
            Logger::debug("Deserializing scene.", LoggerChannel::SCENE);
            _scene->deserialize(sceneJSONContent);
        }

        // Say that the scene is initialized
        Logger::info("Scene loaded.", LoggerChannel::SCENE);
        _scene->setInitialized();
	}



	// Setters and getters
	void WdeSceneManager::setScene(std::unique_ptr<Scene> scene) {
		WDE_PROFILE_FUNCTION();
		WdeSceneManager::get()._sceneToAdd = std::move(scene);
	}
}
