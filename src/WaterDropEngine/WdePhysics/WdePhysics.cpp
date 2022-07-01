#include "WdePhysics.hpp"

namespace wde::physics {
	// Module commands
	WdePhysics::WdePhysics(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::PHYS) << "== Initializing Physics Engine ==" << logger::endl;
	}

	void WdePhysics::start() {
		WDE_PROFILE_FUNCTION();

		logger::log(LogLevel::DEBUG, LogChannel::PHYS) << "== Initialization Done ==" << logger::endl;
	}

	void WdePhysics::tick() {
		WDE_PROFILE_FUNCTION();

	}

	void WdePhysics::cleanUp() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::PHYS) << "== Cleaning Up Physics Engine ==" << logger::endl;

		logger::log(LogLevel::DEBUG, LogChannel::PHYS) << "== Cleaning Up Done ==" << logger::endl;
	}


	// On message from another module, or drawing gui
	void WdePhysics::onNotify(const core::Event& event) {}
}
