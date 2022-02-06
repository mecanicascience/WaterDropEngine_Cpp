#include "WdeGUI.hpp"

namespace wde {
	// Module commands
	WdeGUI::WdeGUI(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Initializing GUI Engine ==" << logger::endl;
		// Init
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Initialization Done ==" << logger::endl;
	}

	void WdeGUI::tick() {
		WDE_PROFILE_FUNCTION();
		// Tick for module
	}

	void WdeGUI::cleanUp() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up GUI Engine ==" << logger::endl;
		// Clean Up
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up Done ==" << logger::endl;
	}


	// On message from another module
	void WdeGUI::onNotify(core::Event event) {
		logger::log() << "Notification received from GUI : " << event.msg << logger::endl;
	}
}
