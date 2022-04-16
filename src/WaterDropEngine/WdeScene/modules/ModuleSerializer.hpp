#pragma once

#include "../../../wde.hpp"
#include "../GameObject.hpp"
#include "MeshRendererModule.hpp"
#include "CameraModule.hpp"
#include "ControllerModule.hpp"

namespace wde::scene {
	/**
	 * This class is responsible of referencing each modules and their labels.
	 */
	class ModuleSerializer {
		public:
			/**
			 * Add a new module to a game object
			 * @param moduleName Name of the module
			 * @param moduleConfig Configuration of the module
			 * @param go Corresponding game object
			 */
			static void addModuleFromName(const std::string& moduleName, const std::string& moduleConfig, GameObject& go) {
				WDE_PROFILE_FUNCTION();
				if (moduleName == "Transform") {
					if (!moduleConfig.empty())
						go.transform->setConfig(moduleConfig);
				}
				else if (moduleName == "Mesh Renderer") {
					if (moduleConfig.empty()) go.addModule<MeshRendererModule>();
					else go.addModule<MeshRendererModule>(moduleConfig);
				}
				else if (moduleName == "Camera") {
					if (moduleConfig.empty()) go.addModule<CameraModule>();
					else go.addModule<CameraModule>(moduleConfig);
				}
				else if (moduleName == "Keyboard Controller") {
					if (moduleConfig.empty()) go.addModule<ControllerModule>();
					else go.addModule<ControllerModule>(moduleConfig);
				}
				else
					throw WdeException(LogChannel::SCENE, "Trying to load a module '" + moduleName + "' that isn't referenced in ModuleSerializer.");
			}

			/**
			 * Remove a module given a name
			 * @param moduleName
			 * @param go
			 */
			static void removeModuleFromName(const std::string moduleName, GameObject& go) {
				WDE_PROFILE_FUNCTION();
				int index = 0;
				for (auto& mod : go.getModules()) {
					if (mod->getName() == moduleName) {
						go.getModules().erase(go.getModules().begin() + index);
						return;
					}
					index++;
				}
			}

			/**
			 * @param go
			 * @return The remaining modules not in the game object
			 */
			static std::vector<std::string> getRemainingModulesList(GameObject& go) {
				WDE_PROFILE_FUNCTION();
				const std::vector<std::string> everyModulesList { "Transform", "Mesh Renderer", "Camera", "Keyboard Controller" };

				// Get modules names in gameobject
				std::vector<std::string> modulesListNames {};
				for (auto& mod : go.getModules())
					modulesListNames.push_back(mod->getName());

				// Compute remaining modules
				std::vector<std::string> remainingModules {};
				for (auto& mod : everyModulesList)
					if (std::find(modulesListNames.begin(), modulesListNames.end(), mod) == modulesListNames.end())
						remainingModules.push_back(mod);

				// Return remaining modules
				return remainingModules;
			}


			/**
			 * Serialize a module
			 * @param mod
			 * @return The module json description
			 */
			static json serializeModule(Module& mod) {
				WDE_PROFILE_FUNCTION();
				json locJSON;
				locJSON["name"] = mod.getName();
				locJSON["data"] = mod.serialize();
				return locJSON;
			}
	};
}
