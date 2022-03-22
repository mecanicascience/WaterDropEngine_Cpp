#include "WdeResourceManager.hpp"

namespace wde::resource {
	// Core methods
	void WdeResourceManager::tick() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RES) << "Ticking for resource manager." << logger::endl;
	}

	void WdeResourceManager::cleanUp() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RES) << "== Cleaning Up Resource Manager ==" << logger::endl;

		// Release every lasting resources
		for (auto& res : _resources)
			res.second.reset();
		_resources.clear();

		logger::log(LogLevel::DEBUG, LogChannel::RES) << "== Cleaning Up Done ==" << logger::endl;
	}

	void WdeResourceManager::onNotify(const core::Event& event) { }
}
