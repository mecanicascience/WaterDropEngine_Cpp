#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeCore/Structure/Observer.hpp"
#include "modules/TransformModule.hpp"
#include "../../WdeRender/commands/CommandBuffer.hpp"
#include "../../WdeRender/buffers/Buffer.hpp"
#include "../../WdeRender/descriptors/DescriptorBuilder.hpp"
#include "materials/Material.hpp"

namespace wde::scene {
	/**
	 * Handles a game object
	 */
	class GameObject {
		public:
			struct GPUGameObjectData {
				/** Game object world space position */
				glm::mat4 transformWorldSpace {1.0f};
				/** Game object collision sphere */
				glm::vec4 collisionSphere {0.0f};
			};


			// Core functions
			/**
			 * Create a new game object with a transform module (default : 0, 0, 0)
			 * @param id Unique GO ID
			 * @param name
			 * @param isStatic
			 */
			GameObject(uint32_t id, std::string name, bool isStatic)
					: _id(id), name(std::move(name)), _isStatic(isStatic) {
				// Add default transform module
				transform = addModule<TransformModule>();
			}


			~GameObject() {
				transform = nullptr;
				_modules.clear();
			}

			void tick() {
				if (!active)
					return;

				WDE_PROFILE_FUNCTION();
				// Tick for modules
				for (auto& mod : _modules)
					mod->tick();
			}


			void drawGUI() {
				WDE_PROFILE_FUNCTION();
				if (_isStatic) {
					ImGui::Text("%s", "This game object is static.");
					ImGui::Dummy(ImVec2(0.0f, 3.0f));
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0.0f, 5.0f));
				}

				// Render module top
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::Text("%s", (std::to_string(_id) + " - " + name).c_str());
				ImGui::PopFont();
				ImGui::Dummy(ImVec2(0.0f, 3.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				// Render Modules GUI
				int count = 0;
				for(auto& module : _modules) {
					// Add small space between modules
					if (count != 0)
						ImGui::Dummy(ImVec2(0.0f, 18.0f));
					count++;

					// Render module
					ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
					ImGui::Dummy(ImVec2(6.0f, 0.0f));
					ImGui::SameLine();
					if (ImGui::CollapsingHeader(module->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
						ImGui::Dummy(ImVec2(0.0f, 1.0f));
						ImGui::PopFont();
						// Render header content
						module->drawGUI();
					}
					else
						ImGui::PopFont();
				}
			}


			// Getters and setters
			uint32_t getID() const { return _id; }
			void setSelected(bool selected) { _isSelected = selected; }
			bool isSelected() { return _isSelected; }
			bool isStatic() { return _isStatic; }
			std::vector<std::unique_ptr<Module>>& getModules() { return _modules; }


			// Modules handlers
			template<typename T, typename ...Args>
			T* addModule(Args ...args) {
				_modules.push_back(std::make_unique<T>(*this, args...));
				return static_cast<T*>(_modules[_modules.size() - 1].get());
			}

			template<typename T>
			T* getModule() {
			    // Iterate over every module
				for (auto& mod : _modules)
					if (dynamic_cast<T*>(mod.get()) != nullptr)
						return static_cast<T*>(mod.get());
				// Not found
				return nullptr;
			}


			// Public GO data
			/** True if the game object is active */
			bool active = true;
			/** Name of the game object */
			std::string name;
			/** Transform module of the game object */
			TransformModule* transform;


		private:
			/** GO description data */
			uint32_t _id;
			/** GO Modules */
			std::vector<std::unique_ptr<Module>> _modules;
			/** True if the game object is static (cannot be changed) */
			bool _isStatic;
			/** If true, this object will record from the input engine if it has a player controller */
			bool _isSelected;
	};
}

