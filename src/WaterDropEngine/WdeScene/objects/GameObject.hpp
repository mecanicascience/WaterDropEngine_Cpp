#pragma once

#include "../../../wde.hpp"
#include "Model.hpp"
#include "../modules/Module.hpp"
#include "../modules/TransformModule.hpp"

namespace wde::scene {
	struct TransformComponent {
		glm::vec3 translation {0.0f, 0.0f, 0.0f};
		glm::vec3 scale {1.0f, 1.0f, 1.0f};
		glm::vec3 rotation = {0.0f, 0.0f, 0.0f};

		// Return the corresponding transform matrix : Translation * Ry * Rx * Rz * scale
		glm::mat4 mat4() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
					{
						scale.x * (c1 * c3 + s1 * s2 * s3),
						scale.x * (c2 * s3),
						scale.x * (c1 * s2 * s3 - c3 * s1),
						0.0f,
					},
					{
						scale.y * (c3 * s1 * s2 - c1 * s3),
						scale.y * (c2 * c3),
						scale.y * (c1 * c3 * s2 + s1 * s3),
						0.0f,
					},
					{
						scale.z * (c2 * s1),
						scale.z * (-s2),
						scale.z * (c1 * c2),
						0.0f,
					},
					{translation.x, translation.y, translation.z, 1.0f}};
		}
	};

	class GameObject {
		public:
			using id_t = unsigned int;
			std::shared_ptr<Model> model {};
			TransformComponent transform {};

			// Constructors
			GameObject() : _objectID(-1) {}; // Creates a dummy empty temporary game object (do not use!)

			// Core creation objects
			static GameObject createGameObject(std::string name) {
				static id_t currentID = 0;
				auto go = GameObject(currentID++, name);
				go.addModule<TransformModule>(); // Add transform component
				return go;
			}

			template<typename T>
			void addModule() {
				_moduleList.push_back(new T());

				// Initialize module
				_moduleList[_moduleList.size() - 1]->initialize();
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
			 * Draws the game object to the screen
			 * @param commandBuffer
			 */
			void render(CommandBuffer &commandBuffer) {
				// Render modules
				for (auto& module : _moduleList)
					module->render(commandBuffer);

				// Render models
				model->bind(commandBuffer);
				model->render();
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

				// Clean up model
				model->cleanUp();
			}


			// Gui functions
			void renderGUI() {
				// Render module top
				ImGui::Text(_objectName.c_str());
				ImGui::Dummy(ImVec2(0.0f, 3.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				// Render Modules GUI
				int count = 0;
				for(auto& module : _moduleList) {
					// Add small space between modules
					if (count != 0)
						ImGui::Dummy(ImVec2(0.0f, 8.0f));
					count++;

					// Render module
					if (ImGui::CollapsingHeader(module->getModuleName().c_str())) {
						// Render header content
						module->renderGUI();
					}
				}
			}


			// Getters and setters
			id_t getID() const { return _objectID; }
			std::string& getName() { return _objectName; }




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

