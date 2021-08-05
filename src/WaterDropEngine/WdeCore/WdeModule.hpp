#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

#include "../WdeCommon/WdeUtils/NonCopyable.hpp"

namespace wde {
	/**
	 * Manages the modules
	 * @tparam BaseModule Represents a module
	 */
	template<typename BaseModule>
	class WdeModuleFactory {
		public:
			virtual ~WdeModuleFactory() = default;

			/** Describes a module */
			class ModuleDescription {
				public:
					std::function<std::unique_ptr<BaseModule>()> _createFun {};
					std::string _moduleName {};
					int _moduleStageLevel {};
			};

			/** Registers modules */
			static std::unordered_map<std::string, ModuleDescription> &getRegistry() {
				static std::unordered_map<std::string, ModuleDescription> modulesHandler;
				return modulesHandler;
			}


			/**
			 * A class describing an engine module
			 * @tparam T The class of a module
			 */
			template<typename T>
			class Module : public BaseModule {
				public:
					enum Stage {
						PRE    = 0,  // Before render stage
						NORMAL = 1,  // Default render stage
						POST   = 2,  // After default render stage
						RENDER = 3,   // When rendering
						POST_RENDER = 4  // After rendering
					};

					static T &get() { return *_moduleInstance; }

				protected:
					/**
					 * Creates a new module singleton and registers it into the module list
					 * @param moduleName The module name
					 * @param moduleStage Init and render stage of the module
					 * @return True in static initialization
					 */
					static bool RegisterModule(std::string moduleName, Stage moduleStage) {
						WdeModuleFactory::getRegistry()[moduleName] = {[]() {
							_moduleInstance = new T();
							return std::unique_ptr<BaseModule>(_moduleInstance);
						}, moduleName, (int) moduleStage};
						return true;
					}

				private:
					/** A unique pointer to the instance of the module */
					inline static T *_moduleInstance = nullptr;
			};
	};



	/**
	 * The interface used to define engine modules
	 */
	class WdeModule : public WdeModuleFactory<WdeModule>, NonCopyable {
		public:
			/** Initialize the module */
			virtual void initialize() = 0;
			/** On engine ticks */
			virtual void tick() = 0;
			/** Clean up the module */
			virtual void cleanUp() = 0;
	};
}
