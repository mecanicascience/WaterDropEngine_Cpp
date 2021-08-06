#pragma once

#include <vector>

#include "../../../wde.hpp"
#include "../objects/GameObject.hpp"

namespace wde::scene {
	class Scene : NonCopyable {
		public:
			// Core functions
			/** Initialize scene */
			virtual void initialize() = 0;

			/** Updates the scene (this should be called from the derived children class) */
			virtual void update() {
				// Updates delta time
				auto newTime = std::chrono::high_resolution_clock::now();
				_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - _currentTime).count();
				_currentTime = newTime;

				// Updates game objects
				for (auto& go : _gameObjects)
					go.update(_deltaTime);
			}

			/** Cleaning up scene */
			virtual void cleanUp() final {
				// Clean up game objects
				for (auto& go : _gameObjects)
					go.cleanUp();
				_gameObjects.clear();
			}


			// GUI functions
			/**
			 * Setup the GUI (can be called multiple times - called by the GUI manager) given the ID of the parent gui.
			 * This function should be called by the children scene
			 * @param parentID The ImGUI id of the parent anchor component
			 */
			virtual void setupGUI(ImGuiID &parentID) {
				// Create a game objects list tab
				ImGuiID dockIDLeft = ImGui::DockBuilderSplitNode(parentID, ImGuiDir_Left, 0.19f, nullptr, &parentID);
				ImGui::DockBuilderDockWindow("Scene Components", dockIDLeft);

				// Create a properties list
				ImGuiID dockIDProperties = ImGui::DockBuilderSplitNode(dockIDLeft, ImGuiDir_Down, 0.75f, nullptr, &dockIDLeft);
				ImGui::DockBuilderDockWindow("Properties", dockIDProperties);
			};

			/** Render the GUI elements (called by the GUI manager) */
			virtual void renderGUI() {
				// Setup scene components list
				ImGui::Begin("Scene Components");
				ImGui::BeginChild("Scene Components Children");
				ImGui::Dummy(ImVec2(0.0f, 0.15f));
				static int selected = -1;
				int objID = 0;
				for (auto& go : _gameObjects) {
					if (ImGui::Selectable(go.getName().c_str(), selected == objID))
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
					_gameObjects.at(_selectedGameObjectID).renderGUI();
				ImGui::End();
			};



			// Getters and setters
			void addGameObject(GameObject& gameObject) { _gameObjects.push_back(gameObject); };
			std::vector<GameObject>& getGameObjects() { return _gameObjects; }


		protected:
			/** @return the delta time since the last update (in seconds) */
			const float getDeltaTime() const { return _deltaTime; }


		private:
			/** The list of the scene game objects */
			std::vector<GameObject> _gameObjects {};
			/** The id of the selected game object */
			int _selectedGameObjectID = 0;

			/** Stores the current time to compute delta time */
			std::chrono::time_point<std::chrono::system_clock> _currentTime = std::chrono::high_resolution_clock::now();
			/** Last computed delta time */
			float _deltaTime = 0;
	};
}
