#include "WdeInstance.hpp"

namespace wde {
	WdeStatus WdeInstance::initialize() {
        WDE_PROFILE_FUNCTION();

        // Setup modules list
        auto& modulesNameMap = WdeModule::getRegistry();
        for (auto& module : modulesNameMap) {
        	auto &&moduleInstance = module.second._createFun();
        	if (!_modulesList.contains(module.second._moduleStageLevel)) // First module at level
        		_modulesList.emplace(module.second._moduleStageLevel, std::vector<std::unique_ptr<WdeModule>>{});

            _modulesList.at(module.second._moduleStageLevel).push_back(std::move(moduleInstance));
        }

		// Initialize modules
		Logger::debug("Initializing modules.", LoggerChannel::MAIN);
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
		for (auto iter = _modulesList.rbegin(); iter != _modulesList.rend(); ++iter)
			for (auto& module : iter->second)
				module->cleanUp();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}
}
