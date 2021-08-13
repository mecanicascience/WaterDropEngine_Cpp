#pragma once

#include "../../../wde.hpp"
#include "Model.hpp"
#include "../modules/Module.hpp"
#include "../modules/TransformModule.hpp"
#include "../gizmos/Gizmo.hpp"
#include "../../WdeRenderingEngine/descriptors/Descriptor.hpp"

namespace wde::scene {
	class GameObject {
		struct GPUGameObjectData {
			alignas(16) glm::mat4 transformWorldSpace {1.0f};
		};


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
			/** Initialize the different game object modules */
			void initialize() {
				WDE_PROFILE_FUNCTION();
				// Add game object descriptor set (binding 1)
				_descriptor->addSet(1, {
					{0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, sizeof(GPUGameObjectData)} // Game object description buffer
				});

				// Initialize modules
				for (auto& module : _moduleList)
					module->initialize();
			}

			/**
			 * Update the game object
			 * @param deltaTime Time since last update (in seconds)
			 */
			void update(float deltaTime) {
				WDE_PROFILE_FUNCTION();

				// Update modules
				for (auto& module : _moduleList)
					module->update(deltaTime);
			}

			/**
			 * Draws the game object to the screen at a given render stage
			 * @param commandBuffer
			 * @param renderStage
			 */
			void render(CommandBuffer &commandBuffer, RenderStage stage) {
				WDE_PROFILE_FUNCTION();

				// Update game object descriptor buffer
				GPUGameObjectData goData {};
				goData.transformWorldSpace = getModule<TransformModule>().getTransform();
				_descriptor->getSet(1).updateBuffer(0, &goData);

				// Render modules
				for (auto& module : _moduleList)
					module->render(commandBuffer, stage);
			}

			/**
			 * Render Game object gizmo
			 * @param gizmo The gizmo instance
			 */
			void renderGizmo(Gizmo &gizmo) {
				WDE_PROFILE_FUNCTION();
				// Render modules gizmo
				for (auto& module : _moduleList)
					module->renderGizmo(gizmo);
			}

			/** Clean up game object */
			void cleanUp() {
				WDE_PROFILE_FUNCTION();
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
				WDE_PROFILE_FUNCTION();
				// Render module top
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::Text((std::to_string(_objectID) + " - " + _objectName).c_str());
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
			std::shared_ptr<Descriptor>& getDescriptor() { return _descriptor; }



		private:
			// GameObject values
			id_t _objectID;
			std::string _objectName;

			// List of game object modules
			std::vector<Module*> _moduleList;
			// The game object sets descriptor
			std::shared_ptr<Descriptor> _descriptor {};

			/**
			 * Creates a new game object
			 * @param objectID
			 */
			explicit GameObject(id_t objectID, std::string &objectName)
				: _objectID(objectID),
				  _objectName(objectName),
				  _descriptor(std::make_shared<Descriptor>()) {}
	};
}

