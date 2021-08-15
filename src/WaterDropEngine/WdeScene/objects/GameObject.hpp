#pragma once

#include "../../../wde.hpp"
#include "Model.hpp"
#include "../objects/models/ModelCube.hpp"
#include "../objects/models/ModelLoader.hpp"
#include "../materials/ColorMaterial.hpp"
#include "../materials/TextureMaterial.hpp"
#include "../modules/Module.hpp"
#include "../modules/TransformModule.hpp"
#include "../modules/CameraModule.hpp"
#include "../modules/ModelModule.hpp"
#include "../modules/TransformControllerModule.hpp"
#include "../gizmos/Gizmo.hpp"
#include "../../WdeRenderingEngine/descriptors/Descriptor.hpp"

namespace wde::scene {
	class GameObject {
		struct GPUGameObjectData {
			alignas(16) glm::mat4 transformWorldSpace {1.0f};
			alignas(16) glm::mat4 normalMatrix {1.0f};
		};


		public:
			using id_t = unsigned int;

			// Constructors
			GameObject() : _objectID(-1) {}; // Creates a dummy empty temporary game object (do not use!)

			// Core creation objects
			/**
			 * Create a new game object
			 * @param name The identifier name of the game object
			 * @param resetCount True if the game object IDs needs to be reset (default : false)
			 * @return The created game object
			 */
			static GameObject createGameObject(std::string name, bool resetCount = false) {
				static id_t currentID = 0;
				if (resetCount)
					currentID = 0;
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
				for (auto& descriptor : _descriptors)
					descriptor->addSet(1, {
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
				goData.normalMatrix = (glm::mat4) getModule<TransformModule>().getNormalMatrix();
				for (auto& descriptor : _descriptors)
					descriptor->getSet(1).updateBuffer(0, &goData);

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


			// Serialization
			/** Serialize the game object */
			json serialize() {
				json serializedGO;
				// Game object properties
				serializedGO["id"] = getID();
				serializedGO["name"] = getName();

				// Serialize modules
				serializedGO["modules"] = json::array();
				for (auto& module : _moduleList) {
					json serializeData = module->serializeModule();
					if (serializeData != json(""))
						serializedGO["modules"].push_back(serializeData);
				}

				// Return json
				return serializedGO;
			}


			static bool nameEquals(std::string s1, std::string s2) {
				return s1 == ("\"" + s2 + "\"");
			}
			/** Deserialize the game object */
			void deserialize(json moduleList) {
				// Create modules
				for (auto& module : moduleList) {
					std::string name = to_string(module["name"]);

					// Set modules
					if (nameEquals(name, "Camera"))
						addModule<CameraModule>();

					else if (nameEquals(name, "Model")) {
						// Model
						std::shared_ptr<Model> model;
						std::string modelType = to_string(module["data"]["model"]["type"]);
						if (nameEquals(modelType, "Cube"))
							model = std::make_shared<ModelCube>();
						else if (nameEquals(modelType, "external"))
							model = std::make_shared<ModelLoader>(module["data"]["model"]["relativePath"]);
						else
							throw WdeException("Failed to serialize model of type " + to_string(module["data"]["model"]["type"]) + ".", LoggerChannel::SCENE);

						// Material
						std::shared_ptr<Material> material;
						std::string materialType = to_string(module["data"]["material"]["type"]);
						RenderStage stage {int(module["data"]["material"]["stage"]["first"]), int(module["data"]["material"]["stage"]["second"])};
						if (nameEquals(materialType, "Color Material")) {
							VkPolygonMode polygMode = VK_POLYGON_MODE_FILL;
							if (int(module["data"]["material"]["mode"]) == VK_POLYGON_MODE_LINE)
								polygMode = VK_POLYGON_MODE_LINE;
							else if (int(module["data"]["material"]["mode"]) == VK_POLYGON_MODE_POINT)
								polygMode = VK_POLYGON_MODE_POINT;
							else if (int(module["data"]["material"]["mode"]) == VK_POLYGON_MODE_FILL_RECTANGLE_NV)
								polygMode = VK_POLYGON_MODE_FILL_RECTANGLE_NV;
							else if (int(module["data"]["material"]["mode"]) == VK_POLYGON_MODE_MAX_ENUM)
								polygMode = VK_POLYGON_MODE_MAX_ENUM;
							material = std::make_shared<ColorMaterial>(stage, polygMode);
						}
						else if (nameEquals(materialType, "Texture Material"))
							material = std::make_shared<TextureMaterial>(stage, module["data"]["material"]["texturePath"], module["data"]["material"]["textureFilter"],
																		 module["data"]["material"]["textureAdressMode"]);
						else
							throw WdeException("Failed to serialize material of type " + to_string(module["data"]["material"]["type"]) + ".", LoggerChannel::SCENE);

						// Module
						addModule<ModelModule>(model, material);
					}

					else if (nameEquals(name, "Transform Controller"))
						addModule<TransformControllerModule>();

					else if (nameEquals(name, "Transform")) {} // Already added when creating object

					else // Module not found
						throw WdeException("Failed to deserialize a module named " + name + " that isn't referenced in the deserialize method.", LoggerChannel::SCENE);

					_moduleList[_moduleList.size() - 1]->deserialize(module["data"]);
				}
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
			std::vector<std::shared_ptr<Descriptor>>& getDescriptors() { return _descriptors; }
			void createDescriptors() {
				for (auto& module : _moduleList)
					module->createDescriptors();
			}

			std::shared_ptr<Descriptor>& createDescriptor() {
				_descriptors.push_back(std::make_shared<Descriptor>());
				return _descriptors[_descriptors.size() - 1];
			}



		private:
			// GameObject values
			id_t _objectID;
			std::string _objectName;

			// List of game object modules
			std::vector<Module*> _moduleList;
			// The game object sets descriptor
			std::vector<std::shared_ptr<Descriptor>> _descriptors {};

			/**
			 * Creates a new game object
			 * @param objectID
			 */
			explicit GameObject(id_t objectID, std::string &objectName)
				: _objectID(objectID), _objectName(objectName) {}
	};
}

