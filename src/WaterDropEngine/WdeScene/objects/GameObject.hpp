#pragma once

#include "../../../wde.hpp"
#include "Model.hpp"
#include "../modules/Module.hpp"
#include "../modules/TransformModule.hpp"

namespace wde::scene {
	class GameObject {
		public:
			using id_t = unsigned int;

			// Constructors
			GameObject() : _objectID(-1) {}; // Creates a dummy empty temporary game object (do not use!)

			// Core creation objects
			static GameObject createGameObject(std::string name) {
				static id_t currentID = 0;
				auto go = GameObject(currentID++, name);
				go.addModule<TransformModule>(); // Add transform component
				return go;
			}


			// Modules handling
			template<typename T, typename ... Args>
			T& addModule(Args ...args) {
				_moduleList.push_back(new T(*this, args...));

				// Initialize module
				_moduleList[_moduleList.size() - 1]->initialize();
				return *dynamic_cast<T*>(_moduleList[_moduleList.size() - 1]);
			}

			template<typename T>
			T& getModule() {
				for (Module* module : _moduleList) {
					if (dynamic_cast<T*>(module) != nullptr)
						return *dynamic_cast<T*>(module);
				}

				// Not found
				Logger::warn("Trying to get an undefined module.", LoggerChannel::SCENE);
				// Try to add this module with no parameters
				return addModule<T>();
			}

			template<typename T>
			bool hasModule() {
				for (Module* module : _moduleList)
					if (dynamic_cast<T*>(module) != nullptr)
						return true;
				return false; // Not found
			}




			// Core functions
			/**
			 * Update the game object
			 * @param deltaTime Time since last update (in seconds)
			 */
			void update(float deltaTime) {
				// Update modules
				for (auto& module : _moduleList)
					module->update(deltaTime);
			}

			/**
			 * Draws the game object to the screen at a given render stage
			 * @param commandBuffer
			 * @param renderStage
			 * @param camera
			 */
			void render(CommandBuffer &commandBuffer, RenderStage stage, GameObject& camera) {
				// Render modules
				for (auto& module : _moduleList)
					module->render(commandBuffer, stage, camera);
			}

			/** Clean up game object */
			void cleanUp() {
				// Clean up modules
				for(auto& module : _moduleList) {
					module->cleanUp();
					delete module;
					module = nullptr;
				}
				_moduleList.clear();
			}


			// Gui functions
			void renderGUI() {
				// Render module top
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::Text(_objectName.c_str());
				ImGui::PopFont();
				ImGui::Dummy(ImVec2(0.0f, 3.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				// Render Modules GUI
				int count = 0;
				for(auto& module : _moduleList) {
					// Add small space between modules
					if (count != 0)
						ImGui::Dummy(ImVec2(0.0f, 18.0f));
					count++;

					// Render module
					ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
					ImGui::Dummy(ImVec2(6.0f, 0.0f));
					ImGui::SameLine();
					if (ImGui::CollapsingHeader(module->getModuleName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
						ImGui::Dummy(ImVec2(0.0f, 1.0f));
						ImGui::PopFont();
						// Render header content
						module->renderGUI();
					}
					else
						ImGui::PopFont();
				}
			}


			// Getters and setters
			id_t getID() const { return _objectID; }
			std::string& getName() { return _objectName; }
			std::vector<Module*>& getModules() { return _moduleList; }



		private:
			// GameObject values
			id_t _objectID;
			std::string _objectName;

			// List of game object modules
			std::vector<Module*> _moduleList;

			/**
			 * Creates a new game object
			 * @param objectID
			 */
			explicit GameObject(id_t objectID, std::string &objectName) : _objectID(objectID), _objectName(objectName) {}
	};
}

