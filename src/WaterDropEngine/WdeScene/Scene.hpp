#pragma once

#include <vector>
#include <map>

#include "../../wde.hpp"
#include "objects/GameObject.hpp"
#include "modules/CameraModule.hpp"
#include "gizmos/Gizmo.hpp"

namespace wde::scene {
	class Scene : NonCopyable {
		public:
			explicit Scene(std::string sceneName) : _sceneName(std::move(sceneName)) {}

			// Core functions
			/** Initialize scene */
			virtual void initialize() {};
			/** Initialize scene game objects */
			virtual void initializeGameObjects() final {
				WDE_PROFILE_FUNCTION();
				for (auto& go : _gameObjects) {
					// Add game object scene descriptor set (binding 0)
					go->getDescriptor()->addSet(0, {
						{0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, sizeof(CameraModule::GPUCameraData)} // Camera buffer
					});

					// Initialize game objects
					go->initialize();
				}
			}

			/** Updates the scene (this should be called from the derived children class) */
			virtual void update() {
				WDE_PROFILE_FUNCTION();
				// Updates delta time
				auto newTime = std::chrono::high_resolution_clock::now();
				_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - _currentTime).count();
				_currentTime = newTime;

				// Updates game objects and updates optional camera ID if there is no camera
				for (int i = 0; i < _gameObjects.size(); i++) {
					_gameObjects[i]->update(_deltaTime);

					if (_sceneCameraID == -1 && _gameObjects[i]->hasModule<CameraModule>()) { // No camera but go has a camera component
						Logger::debug("Selecting main camera as game object named \"" + _gameObjects[i]->getName() + "\".", LoggerChannel::SCENE);
						_sceneCameraID = i;
					}
				}

				if (_sceneCameraID == -1)
					Logger::warn("Camera not found. Consider adding a camera to your scene.", LoggerChannel::SCENE);
			}

			/** Renders scene game objects */
			virtual void render(CommandBuffer& commandBuffer, RenderStage stage) final {
				WDE_PROFILE_FUNCTION();

				// Create scene camera buffer
				CameraModule::GPUCameraData camData = getCamera().getModule<scene::CameraModule>().getCameraData();

				for (auto& go : _gameObjects) {
					// Update game object scene set
					go->getDescriptor()->getSet(0).updateBuffer(0, &camData);

					// Render game object
					go->render(commandBuffer, stage);
				}
			}

			/** Render scene game objects Gizmo */
			virtual void renderGizmo(Gizmo &gizmo) final {
				WDE_PROFILE_FUNCTION();
				for (auto& go : _gameObjects)
					go->renderGizmo(gizmo);
			}

			/** Cleaning up scene */
			virtual void cleanUp() final {
				WDE_PROFILE_FUNCTION();
				// Clean up game objects
				for (auto& go : _gameObjects) {
					go->cleanUp();
					go.reset();
				}
				_gameObjects.clear();
			}

			/** Serialize the scene */
			json serialize() {
				WDE_PROFILE_FUNCTION();

				// Initial json string
				json sceneJSON;
				sceneJSON["name"] = _sceneName;

				// Serialize game objects
				sceneJSON["gameObjects"] = json::array();
				for (auto& go : _gameObjects)
					sceneJSON["gameObjects"].push_back(go->serialize());

				return sceneJSON;
			}

			/** Deserialize the scene from a json string */
			void deserialize(const json& sceneContent) {
				WDE_PROFILE_FUNCTION();
				Logger::info("Loading scene " + _sceneName + ".", LoggerChannel::SCENE);

				// Create game objects
				auto& gameObjectList = sceneContent["gameObjects"];

				// Sort by ID
				std::unordered_map<int, json> gameObjectIDs {};
				int maxID = 0;
				for (auto& go : gameObjectList) {
					int ID = int(go["id"]);
					if (ID > maxID)
						maxID = ID;
					gameObjectIDs[ID] = go;
				}

				// Create game objects and deserialize them
				bool firstObject = true;
				for (int i = 0; i < maxID + 1; i++) {
					if (gameObjectIDs.contains(i)) {
						createGameObject(gameObjectIDs[i]["name"], firstObject);
						_gameObjects[_gameObjects.size() - 1]->deserialize(gameObjectIDs[i]["modules"]);
						firstObject = false;
					}
				}

				// Initialize game objects
				initializeGameObjects();
			}


			// GUI functions
			/**
			 * Setup the GUI (can be called multiple times - called by the GUI manager) given the ID of the parent gui.
			 * This function should be called by the children scene
			 * @param parentID The ImGUI id of the parent anchor component
			 */
			virtual void setupGUI(ImGuiID &parentID) {
				WDE_PROFILE_FUNCTION();
				// Create a game objects list tab
				ImGuiID dockIDLeft = ImGui::DockBuilderSplitNode(parentID, ImGuiDir_Left, 0.19f, nullptr, &parentID);
				ImGui::DockBuilderDockWindow("Scene Components", dockIDLeft);

				// Create a properties list
				ImGuiID dockIDProperties = ImGui::DockBuilderSplitNode(dockIDLeft, ImGuiDir_Down, 0.75f, nullptr, &dockIDLeft);
				ImGui::DockBuilderDockWindow("Properties", dockIDProperties);
			};

			/** Render the GUI elements (called by the GUI manager) */
			virtual void renderGUI() {
				WDE_PROFILE_FUNCTION();
				// Setup scene components list
				ImGui::Begin("Scene Components");
				ImGui::BeginChild("Scene Components Children");
				ImGui::Dummy(ImVec2(0.0f, 0.15f));
				static int selected = -1;
				int objID = 0;
				for (auto& go : _gameObjects) {
					ImGui::Dummy(ImVec2(0.2f, 0.0f));
					ImGui::SameLine();
					if (ImGui::Selectable((std::to_string(go->getID()) + " - " + go->getName()).c_str(), selected == objID))
						selected = objID;
					objID++;
				}
				_selectedGameObjectID = selected;
				ImGui::EndChild();
				ImGui::End();

				// Render selected game object properties in properties component
				ImGui::Begin("Properties");
				ImGui::Dummy(ImVec2(0.0f, 0.15f));
				if (_selectedGameObjectID != -1)
					_gameObjects.at(_selectedGameObjectID)->renderGUI();
				ImGui::End();
			};



			// Getters and setters
			/**
			 * Create a new game object and add it to the scene
			 * @param name The identifier name of the game object
			 * @param resetCount True if the game object IDs needs to be reset (default : false)
			 * @return The created game object
			 */
			GameObject& createGameObject(const std::string& objectName, bool resetCount = false) {
				_gameObjects.push_back(std::make_unique<GameObject>(GameObject::createGameObject(objectName, resetCount)));
				return *_gameObjects[_gameObjects.size() - 1];
			}
			std::vector<std::unique_ptr<GameObject>>& getGameObjects() { return _gameObjects; }
			bool hasCamera() { return _sceneCameraID != -1; }
			GameObject& getCamera() { return *_gameObjects[_sceneCameraID]; }
			std::string& getName() { return _sceneName; }


		protected:
			/** @return the delta time since the last update (in seconds) */
			const float getDeltaTime() const { return _deltaTime; }


		private:
			/** The scene name */
			std::string _sceneName;

			/** The list of the scene game objects */
			std::vector<std::unique_ptr<GameObject>> _gameObjects {};
			/** The id of the selected game object */
			int _selectedGameObjectID = 0;
			/** The optional id of the scene camera */
			int _sceneCameraID = -1;

			/** Stores the current time to compute delta time */
			std::chrono::time_point<std::chrono::system_clock> _currentTime = std::chrono::high_resolution_clock::now();
			/** Last computed delta time */
			float _deltaTime = 0;
	};

	/**
	 * Class that represents a scene loaded from a file
	 */
	class LoadedScene : public Scene {
		public:
			/**
			 * Create a new loaded scene given a new
			 * @param name
			 */
			LoadedScene(const json& sceneData) : _sceneData(sceneData), Scene(sceneData["name"]) {}

		private:
			/** The loaded json scene data */
			json _sceneData;
	};
}
