#include "WdeScene.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	bool WdeScene::_showSceneLoadPopup = true;
	bool WdeScene::_showSceneLoadPopupNext = false;

	WdeScene::WdeScene(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Initializing Scene Engine ==" << logger::endl;

		// Load scene
		std::string path;
		auto content = WdeFileUtils::readFileDialog("json", path);
		if (path.empty())
			return;
		auto fileData = json::parse(content);
		path = "res/" + fileData["folderName"].get<std::string>() + "/";

		// Create empty scene
		auto scene = std::make_shared<WdeSceneInstance>();
		scene->setPath(path);
		WaterDropEngine::get().getInstance().setScene(scene);

		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Initialization Done ==" << logger::endl;
	}

	void WdeScene::tick() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Ticking for scene engine." << logger::endl;
	}

	void WdeScene::cleanUp() {
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Cleaning Up Scene Engine ==" << logger::endl;

		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Cleaning Up Done ==" << logger::endl;
	}

	void WdeScene::onNotify(const core::Event& event) {
#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			if (_showSceneLoadPopupNext) {
				ImGui::OpenPopup("SaveScenePopup");
				_showSceneLoadPopupNext = false;
			}

			if (ImGui::BeginPopupModal("SaveScenePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Dummy(ImVec2(0.0f, 3.0f));
				ImGui::Text("Do you want to save the current scene data?");
				ImGui::Dummy(ImVec2(0.0f, 3.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 3.0f));

				if (ImGui::Button("  Do not save  ", ImVec2(120, 0))) {
					_showSceneLoadPopup = false;
					loadScenePath();
					loadScene();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(5.0f, 0.0f));
				ImGui::SameLine();

				if (ImGui::Button("  Save  ", ImVec2(120, 0))) {
					saveScene();
					_showSceneLoadPopup = false;
					loadScenePath();
					loadScene();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
#endif
	}


	void WdeScene::loadScenePath() {
		WDE_PROFILE_FUNCTION();
		// Save last scene
#ifdef WDE_GUI_ENABLED
		if (_showSceneLoadPopup) {
			_showSceneLoadPopupNext = true;
			return;
		}
		_showSceneLoadPopupNext = false;
		_showSceneLoadPopup = true;
#endif

		// Get file name
		std::string path;
		auto content = WdeFileUtils::readFileDialog("json", path);
		if (path.empty())
			return;
		auto fileData = json::parse(content);
		path = "res/" + fileData["folderName"].get<std::string>() + "/";

		// Kill last scene
		WaterDropEngine::get().getInstance().getScene()->cleanUp();

		// Create empty scene
		auto scene = std::make_shared<WdeSceneInstance>();
		scene->setPath(path);
		WaterDropEngine::get().getInstance().setScene(scene);
	}

	void WdeScene::loadScene() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Loading scene data." << logger::endl;

		// Load scene data
		auto& resourceManager = WaterDropEngine::get().getResourceManager();
		auto scene = WaterDropEngine::get().getInstance().getScene();

		// Load scene data
		auto path = scene->getPath();
		auto fileData = json::parse(WdeFileUtils::readFile(path + "scene.json"));
		if (fileData["type"] != "scene")
			throw WdeException(LogChannel::SCENE, "Trying to load a non-scene JSON object.");

		// Load materials
		for (const auto& materialIDs : fileData["data"]["materials"])
			resourceManager.load<resource::Material>(path + "description/materials/" + materialIDs.get<std::string>());

		// Load game objects
		uint32_t currentGOID = scene->getGameObjects().size();
		for (const auto& goIDs : fileData["data"]["gameObjects"]) {
			const auto& goData = json::parse(WdeFileUtils::readFile(path + "description/gameObjects/go_" + std::to_string(goIDs.get<uint32_t>()) + ".json"));
			if (goData["type"] != "gameObject")
				throw WdeException(LogChannel::SCENE, "Trying to load a non-gameObject resource type as a gameObject.");

			// Create game object
			auto go = scene->createGameObject(goData["name"], goData["data"]["static"].get<bool>());
			go->active = goData["data"]["active"].get<bool>();

			// Create game object modules
			for (const auto& modData : goData["modules"]) {
				auto modConf = to_string(modData["data"]);
				if (modData["name"] == "Transform")
					go->transform->setConfig(modConf);
				else if (modData["name"] == "MeshRenderer")
					go->addModule<MeshRendererModule>(modConf);
				else if (modData["name"] == "Camera")
					go->addModule<CameraModule>(modConf);
				else if (modData["name"] == "KeyboardController")
					go->addModule<ControllerModule>(modConf);
				else
					throw WdeException(LogChannel::SCENE, "Trying to load a module '" + modData["name"].get<std::string>() + "' that isn't referenced in WdeScene.");
			}
		}

		// Set game object parents and children
		for (const auto& goIDs : fileData["data"]["gameObjects"]) {
			const auto& goData = json::parse(WdeFileUtils::readFile(path + "description/gameObjects/go_" + std::to_string(goIDs.get<uint32_t>()) + ".json"));
			if (goData["modules"][0]["name"] == "Transform"
					&& goData["modules"][0]["data"]["parentID"].get<int>() != -1) // First module should always be the transform module
				scene->getGameObject((int)currentGOID)->transform->setParent(scene->getGameObject(goData["modules"][0]["data"]["parentID"].get<int>())->transform);
			currentGOID++;
		}
	}

	void WdeScene::saveScene() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Saving scene data." << logger::endl;

	}
}
