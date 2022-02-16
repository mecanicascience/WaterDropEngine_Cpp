#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeCore/Structure/Observer.hpp"
#include "modules/TransformModule.hpp"

namespace wde::scene {
	/**
	 * Handles a game object
	 */
	class GameObject {
		public:
			// Core functions
			/**
			 * Create a new game object with a transform module (default : 0, 0, 0)
			 * @param id Unique GO ID
			 * @param name
			 */
			GameObject(uint32_t id, std::string name)
					: _id(id), name(std::move(name)) {
				transform = addModule<TransformModule>();
			}
			~GameObject() {
				transform = nullptr;
				_modules.clear();
			}

			void tick() {
				for (auto& mod : _modules)
					mod->tick();
			}

			void drawGUI() {
				WDE_PROFILE_FUNCTION();
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


			// Modules handlers
			template<typename T, typename ...Args>
			T* addModule(Args ...args) {
				_modules.push_back(std::make_unique<T>(*this, args...));
				return static_cast<T*>(_modules[_modules.size() - 1].get());
			}

			template<typename T>
			T* getModule() {
				for (auto& mod : _modules)
					if (static_cast<T*>(mod.get()) != nullptr)
						return static_cast<T*>(mod.get());

				// Not found
				logger::log(LogLevel::WARN, LogChannel::SCENE) << "Trying to get an undefined module." << logger::endl;
				// Try to add this module of this type with no parameters
				return addModule<T>();
			}


			// Public GO data
			/** Name of the game object */
			std::string name;
			/** Transform module of the game object */
			TransformModule* transform;


		private:
			// GO description data
			uint32_t _id;
			// GO Modules
			std::vector<std::unique_ptr<Module>> _modules;
	};
}

