#include "WdeInstance.hpp"

namespace wde {
	void WdeInstance::createModules() {
		WDE_PROFILE_FUNCTION();

		// Setup modules list
		auto& modulesNameMap = WdeModule::getRegistry();
		for (auto& module : modulesNameMap) {
			auto &&moduleInstance = module.second._createFun();
			if (!_modulesList.contains(module.second._moduleStageLevel)) // First module at level
				_modulesList.emplace(module.second._moduleStageLevel, std::vector<std::unique_ptr<WdeModule>>{});

			_modulesList.at(module.second._moduleStageLevel).push_back(std::move(moduleInstance));
		}
	}

	WdeStatus WdeInstance::initialize() {
        WDE_PROFILE_FUNCTION();

		// Initialize modules
		Logger::debug("Initializing modules.", LoggerChannel::MAIN);
		// Always initialize the render module first
		int RENDER_MODULE_ID = WdeModule::Module<WdeInstance>::Stage::RENDER;
		for (auto& module : _modulesList.at(RENDER_MODULE_ID))
			module->initialize();

		// Initialize other modules
		for (auto& [id, modules] : _modulesList)
			for (auto& module : modules)
				module->initialize();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::run() {
        WDE_PROFILE_FUNCTION();

		// Main game loop
		while (!renderEngine::WdeRenderEngine::get().getCoreWindow().shouldClose()) { // Ask the render engine if we need to quit
			Logger::debug("====== Updating new frame. ======", LoggerChannel::MAIN);

			// Tick for modules
			Logger::debug("Ticking for modules.", LoggerChannel::MAIN);
			for (auto& [id, modules] : _modulesList)
				for (auto& module : modules)
					module->tick();

			Logger::debug("====== End of tick. ======\n\n", LoggerChannel::MAIN);
		}

		// Wait until every used device are ready
		renderEngine::WdeRenderEngine::get().waitForDevicesReady();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}



	WdeStatus WdeInstance::cleanUp() {
	    WDE_PROFILE_FUNCTION();

		// Clean-up modules in reverse order
		Logger::debug("Cleaning up modules.", LoggerChannel::MAIN);

		// Clean up modules but not render engine
		int RENDER_MODULE_ID = WdeModule::Module<WdeInstance>::Stage::RENDER;
		for (auto iter = _modulesList.rbegin(); iter != _modulesList.rend(); ++iter)
			if (iter->first != RENDER_MODULE_ID)
				for (auto& module : iter->second)
					module->cleanUp();

		// Clean up render engine
		for (auto& module : _modulesList.at(RENDER_MODULE_ID))
			module->cleanUp();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}
}
