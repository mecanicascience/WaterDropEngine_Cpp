#include "Chunk.hpp"

#include <utility>
#include "../../WaterDropEngine.hpp"
#include "../../WdeScene/WdeSceneInstance.hpp"

namespace wde::scene {
	Chunk::Chunk(WdeSceneInstance* sceneInstance, glm::ivec2 pos) : _sceneInstance(sceneInstance), _pos(pos) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Loading chunk (" << _pos.x << ", " << _pos.y << ")." << logger::endl;

		// Check if file chunk exist, if not create empty chunk
		auto path = _sceneInstance->getPath();
		bool exist = WdeFileUtils::fileExist(path + "chunk/chunk_" + std::to_string(pos.x) + "-" + std::to_string(pos.y) + ".json");

		// Add editor camera if chunk (0, 0)
#ifdef WDE_ENGINE_MODE_DEBUG
		if (_pos.x == 0 && _pos.y == 0) {
			auto camera = createGameObject("Editor Camera");
			auto camModule = camera->addModule<scene::CameraModule>();
			camModule->setAsActive();
			camModule->setFarPlane(std::numeric_limits<float>::max());
			WaterDropEngine::get().getInstance().getScene()->setActiveCamera(camera.get());
			camera->addModule<scene::ControllerModule>();
			camera->transform->position = glm::vec3 {0.0f, 0.0f, 0.0f};
			camera->transform->rotation = glm::vec3 {0.0f, 0.0f, 0.0f};
		}
#endif

		// No chunk data
		if (!exist)
			return;

		// Check chunk file format
		auto fileData = json::parse(WdeFileUtils::readFile(path + "chunk/chunk_" + std::to_string(pos.x) + "-" + std::to_string(pos.y) + ".json"));
		if (fileData["type"] != "chunk")
			throw WdeException(LogChannel::SCENE, "Trying to load a non-chunk JSON object.");
		if (fileData["data"]["id"]["x"].get<int>() != pos.x || fileData["data"]["id"]["y"].get<int>() != pos.y)
			throw WdeException(LogChannel::SCENE, "Chunk at (" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ") has incorrect ID in JSON file.");

		// Load chunk game objects
		uint32_t currentGOID = _gameObjectsIDCurr;
		std::unordered_map<uint32_t, uint32_t> oldToNewIds {}; // <oldID, newID>
		for (const auto& goData : fileData["data"]["gameObjects"]) {
			if (goData["type"] != "gameObject")
				throw WdeException(LogChannel::SCENE, "Trying to load a non-gameObject resource type as a gameObject.");

			// Create game object
			auto go = createGameObject(goData["name"], goData["data"]["static"].get<bool>());
			go->active = goData["data"]["active"].get<bool>();

			// Add parent id to list
			oldToNewIds.emplace(goData["data"]["id"].get<uint32_t>(), go->getID());

			// Create game object modules
			for (const auto& modData : goData["modules"])
				ModuleSerializer::addModuleFromName(modData["name"], to_string(modData["data"]), *go);
		}

		// Set game object parents and children
		for (const auto& goData : fileData["data"]["gameObjects"]) {
			if (goData["modules"][0]["name"] == "Transform" && goData["modules"][0]["data"]["parentID"].get<int>() != -1) // First module should always be the transform module
				_gameObjects[(int) currentGOID]->transform->setParent(_gameObjects[oldToNewIds.at(goData["modules"][0]["data"]["parentID"].get<int>())]->transform);
			currentGOID++;
		}
	}

	void Chunk::save() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Saving chunk (" << _pos.x << ", " << _pos.y << ")." << logger::endl;

		// Make sure directory is created
		std::filesystem::create_directories(_sceneInstance->getPath() + "chunk/");

		// Chunk data
		json chunkData {};
		chunkData["type"] = "chunk";
		chunkData["data"]["id"]["x"] = _pos.x;
		chunkData["data"]["id"]["y"] = _pos.y;

		// Game objects list
		std::vector<json> goJSONArr {};
#ifdef WDE_ENGINE_MODE_DEBUG
		if (_gameObjects.size() >= 0 && _gameObjects[0]->name == "Editor Camera")
			goJSONArr.resize(_gameObjects.size() - 1);
		else
			goJSONArr.resize(_gameObjects.size());
#else
		goJSONArr.resize(_gameObjects.size());
#endif

		int it = 0;
		for (const auto& res : _gameObjects) {
			// Continue if editor camera
#ifdef WDE_ENGINE_MODE_DEBUG
			if (res->name == "Editor Camera")
				continue;
#endif

			// Create GO file
			json goJSON;
			goJSON["type"] = "gameObject";
			goJSON["name"] = res->name;
			goJSON["data"]["id"] = res->getID();
			goJSON["data"]["active"] = res->active;
			goJSON["data"]["static"] = res->isStatic();

			// Create modules json data
			std::vector<json> modulesJSON;
			for (auto& mod : res->getModules())
				modulesJSON.push_back(ModuleSerializer::serializeModule(*mod));
			goJSON["modules"] = modulesJSON;

			// Output file
			goJSONArr[it++] = goJSON;
		}
		chunkData["data"]["gameObjects"] = goJSONArr;

		// Serialize and write to file
		std::ofstream outputData {_sceneInstance->getPath() + "chunk/chunk_" + std::to_string(_pos.x) + "-" + std::to_string(_pos.y) +  ".json", std::ofstream::out};
		outputData << to_string(chunkData);
		outputData.close();
	}

	Chunk::~Chunk() {
		WDE_PROFILE_FUNCTION();

		// Save chunk data
		save();

		// Remove game objects
		_sceneInstance = nullptr;
		_gameObjectsDynamic.clear();
		_gameObjectsStatic.clear();
		_gameObjects.clear();
	}


	void Chunk::tick() {
		WDE_PROFILE_FUNCTION();

		// Remove game objects to delete
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Removing deleted game objects." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::deleteGameObjects");

			auto sceneInstance = WaterDropEngine::get().getInstance().getScene();
			if (!_gameObjectsToDelete.empty()) {
				// Remove selected and active camera
				for (GameObject* go : _gameObjectsToDelete) {
					if (sceneInstance->getActiveGameObject() == go)
						sceneInstance->getActiveGameObject() = nullptr;
					if (sceneInstance->getActiveCamera() == go)
						sceneInstance->setActiveCamera(nullptr);
				}

				// Remove from static list
				_gameObjectsStatic.erase(std::remove_if(_gameObjectsStatic.begin(), _gameObjectsStatic.end(), [this](const auto&x) {
					return std::find(_gameObjectsToDelete.begin(), _gameObjectsToDelete.end(), x.get()) != _gameObjectsToDelete.end();
				}), _gameObjectsStatic.end());

				// Remove from dynamic list
				_gameObjectsDynamic.erase(std::remove_if(_gameObjectsDynamic.begin(), _gameObjectsDynamic.end(), [this](const auto&x) {
					return std::find(_gameObjectsToDelete.begin(), _gameObjectsToDelete.end(), x.get()) != _gameObjectsToDelete.end();
				}), _gameObjectsDynamic.end());

				// Remove from game objects
				_gameObjects.erase(std::remove_if(_gameObjects.begin(), _gameObjects.end(), [this](const auto&x) {
					return std::find(_gameObjectsToDelete.begin(), _gameObjectsToDelete.end(), x.get()) != _gameObjectsToDelete.end();
				}), _gameObjects.end());

				// Clear game objects to delete
				_gameObjectsToDelete.clear();
			}
		}

		// Update game objects
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Ticking for scene dynamic game objects." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::tick::dynamicGameObjects");

			for (auto &go: _gameObjectsDynamic)
				go->tick();
		}
	}



	void Chunk::drawGUI() {
#ifdef WDE_GUI_ENABLED
		WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::drawGUI");
		if (_gameObjects.empty())
			return;

		// Setup scene components list
		gui::GUIRenderer::pushWindowTabStyle();
		ImGui::Begin("Scene Components");
		gui::GUIRenderer::popWindowTabStyle();
		ImGui::BeginChild("Scene Components Children");
		ImGui::Dummy(ImVec2(0.0f, 0.15f));

		// Add Game Object button
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		if (ImGui::Button(ICON_FA_PLUS_CIRCLE))
			createGameObject("Empty Gameobject");
		ImGui::PopFont();
		ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
		ImGui::PushFont(ImGui::GetIO().FontDefault);
		ImGui::SameLine();
		ImGui::Text("Add an empty Gameobject");
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::Separator();

		// Scene game objects
		ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoClip;
		auto scene = WaterDropEngine::get().getInstance().getScene();
		GameObject* oldSelected = scene->getActiveGameObject();
		if (ImGui::BeginTable("Game Objects List", 3, flags)) {
			// Draw game objects list
			for (auto& go : _gameObjects) {
				if (go->transform->getParent() == nullptr) {
					ImGui::TableNextRow();
					drawGUIForGo(go, scene->getActiveGameObject());
				}
			}
			ImGui::EndTable();
		}

		// Selected game object changed
		if (oldSelected != scene->getActiveGameObject()) {
			if (oldSelected != nullptr)
				oldSelected->setSelected(false);
			if (scene->getActiveGameObject() != nullptr)
				scene->getActiveGameObject()->setSelected(true);
		}

		ImGui::EndChild();
		ImGui::End();



		// Render selected game object properties in properties component
		gui::GUIRenderer::pushWindowTabStyle();
		ImGui::Begin("Properties");
		gui::GUIRenderer::popWindowTabStyle();
		ImGui::PushFont(ImGui::GetIO().FontDefault);
		ImGui::Dummy(ImVec2(0.0f, 0.15f));
		if (scene->getActiveGameObject() != nullptr)
			scene->getActiveGameObject()->drawGUI();
		ImGui::End();
		ImGui::PopFont();
#endif
	}





	void Chunk::drawGUIForGo(const std::shared_ptr<GameObject> &go, GameObject*& selected) const {
#ifdef WDE_GUI_ENABLED
		WDE_PROFILE_FUNCTION();
		std::string typeName;
		if (go->getModule<MeshRendererModule>())
			typeName = "Mesh Entity";
		else if (go->getModule<CameraModule>())
			typeName = "Camera";

		// Enable / disabled icon
		ImGui::TableSetColumnIndex(0);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		bool notAct = false;
		if (!go->active) {
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			notAct = true;
		}
		ImGui::PushID(static_cast<int>(go->getID()) + 216846351);
		auto textS = ImGui::CalcTextSize("      ");
		if ((go->active && ImGui::Selectable(" " ICON_FA_EYE, false, 0, textS)) || (!go->active && ImGui::Selectable(" " ICON_FA_EYE_SLASH, false, 0, textS)))
			go->active = !go->active;
		ImGui::PopID();
		if (notAct)
			ImGui::PopStyleColor();
		ImGui::PopFont();


		// Content
		ImGui::TableSetColumnIndex(1);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::PushID(static_cast<int>(go->getID()) + 216846352);
		bool hasNode = false;
		if (!go->transform->getChildrenIDs().empty() && ImGui::TreeNode("")) {
			hasNode = true;
			// Compute buffer without offset
			char buf3[4 + go->name.size() + 5];
			if (typeName == "Mesh Entity")
				sprintf(buf3, ICON_FA_GHOST "  %s", go->name.c_str());
			else if (typeName == "Camera")
				sprintf(buf3, ICON_FA_CAMERA "  %s", go->name.c_str());
			else
				sprintf(buf3, ICON_FA_FOLDER_OPEN "  %s", go->name.c_str());

			// Draw tree node
			ImGui::SameLine();
			ImGui::PushID(static_cast<int>(go->getID()) + 216846353);
			if (ImGui::Selectable(buf3, selected == go.get(), ImGuiSelectableFlags_SpanAllColumns))
				selected = go.get();
			ImGui::PopID();
			ImGui::PopFont();

			// Type name
			ImGui::TableSetColumnIndex(2);
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			if (typeName.size() > 3)
				ImGui::Text("%s", ((go->isStatic() ? "Static " : "") + typeName + "   ").c_str());
			ImGui::PopStyleColor();

			// Draw for children
			for (int childID : go->transform->getChildrenIDs()) {
				ImGui::TableNextRow();
				drawGUIForGo(_gameObjects[childID], selected);
			}

			ImGui::TreePop();
		}
		ImGui::PopID();

		if (!hasNode) {
			char buf2[4 + go->name.size() + 5];
			std::string extraSpace;
			if (go->transform->getChildrenIDs().empty())
				extraSpace = "     ";

			if (typeName == "Mesh Entity")
				sprintf(buf2, (extraSpace + " " + ICON_FA_GHOST "   %s").c_str(), go->name.c_str());
			else if (typeName == "Camera")
				sprintf(buf2, (extraSpace + " " + ICON_FA_CAMERA "   %s").c_str(), go->name.c_str());
			else
				sprintf(buf2, (extraSpace + " " + ICON_FA_FOLDER "   %s").c_str(), go->name.c_str());

			ImGui::SameLine();
			ImGui::PushID(static_cast<int>(go->getID()) + 216846354);
			if (ImGui::Selectable(buf2, selected == go.get(), ImGuiSelectableFlags_SpanAllColumns))
				selected = go.get();
			ImGui::PopID();
			ImGui::PopFont();

			// Type name
			ImGui::TableSetColumnIndex(2);
			ImGui::PushStyleColor(ImGuiCol_Text, gui::GUITheme::colorGrayMinor);
			if (typeName.size() > 3)
				ImGui::Text("%s", ((go->isStatic() ? "Static " : "") + typeName).c_str());
			ImGui::PopStyleColor();
		}
#endif
	}
}