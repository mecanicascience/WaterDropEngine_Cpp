#include "WdeScene.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	bool WdeScene::_showSceneLoadPopup = true;
	bool WdeScene::_showSceneLoadPopupNext = false;

	WdeScene::WdeScene(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		WDE_PROFILE_FUNCTION();
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
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Cleaning Up Scene Engine ==" << logger::endl;

		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Cleaning Up Done ==" << logger::endl;
	}

	void WdeScene::onNotify(const core::Event& event) {
#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeScene::drawGUI()");
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
					WaterDropEngine::get().getGUI().addObserver(WaterDropEngine::get().getInstance().getScene());
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
					WaterDropEngine::get().getGUI().addObserver(WaterDropEngine::get().getInstance().getScene());
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
		scene->setName(fileData["name"]);
	}

	void WdeScene::saveScene() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Saving scene data." << logger::endl;

		// Scene main data
		/*auto scene = WaterDropEngine::get().getInstance().getScene(); // TODO
		json sceneData;
		sceneData["type"] = "scene";
		sceneData["name"] = scene->getName();
		sceneData["folderName"] = scene->getPath().substr(4, scene->getPath().size() - 5);
		std::string sceneRes = "res/" + sceneData["folderName"].get<std::string>() + "/";

		// Create folders
		std::filesystem::create_directories(sceneRes);
		std::filesystem::create_directories(sceneRes + "gameObjects/");

		// Game objects list
		auto goList = std::vector<uint32_t>();
		for (auto& res : scene->getGameObjects()) {
			// Continue if editor camera
#ifdef WDE_ENGINE_MODE_DEBUG
			if (res->getID() == 0)
				continue;
#endif

			// Create reference in scene
			auto index = res->getID();
			goList.push_back(index);

			// Create GO file
			json goJSON;
			goJSON["type"] = "gameObject";
			goJSON["name"] = res->name;
			goJSON["data"]["active"] = res->active;
			goJSON["data"]["static"] = res->isStatic();

			// Create modules json data
			std::vector<json> modulesJSON;
			for (auto& mod : res->getModules())
				modulesJSON.push_back(ModuleSerializer::serializeModule(*mod));
			goJSON["modules"] = modulesJSON;

			// Output file
			std::ofstream outputGOData {sceneRes + "gameObjects/go_" + std::to_string(index) + ".json", std::ofstream::out};
			outputGOData << goJSON << std::endl;
		}
		//sceneData["data"]["gameObjects"] = goList;

		// Serialize and write to file
		std::ofstream outputData {sceneRes + "scene.json", std::ofstream::out};
		outputData << to_string(sceneData);
		outputData.close();*/
	}
}
