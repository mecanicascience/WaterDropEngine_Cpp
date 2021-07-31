#include "WdeInstance.hpp"

namespace wde {
	WdeStatus WdeInstance::initialize() {
        WDE_PROFILE_FUNCTION();

        // Setup modules list
        auto& modulesNameMap = WdeModule::getRegistry();
        for (auto& module : modulesNameMap) {
        	auto &&moduleInstance = module.second._createFun();
        	_modulesList.emplace(module.second._moduleStageLevel, std::move(moduleInstance));
        }

		// Initialize modules
		for (auto& [id, module] : _modulesList)
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
			for (auto& [id, module] : _modulesList)
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
		for (auto iter = _modulesList.rbegin(); iter != _modulesList.rend(); ++iter)
			iter->second->cleanUp();

		// Returns Success
		return WdeStatus::WDE_SUCCESS;
	}
}
