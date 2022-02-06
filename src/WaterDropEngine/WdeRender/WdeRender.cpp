#include "WdeRender.hpp"

namespace wde {
	// Module commands
	WdeRender::WdeRender(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Initializing Rendering Engine ==" << logger::endl;
		// Init
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Initialization Done ==" << logger::endl;
	}

	void WdeRender::tick() {
		// Tick for module
	}

	void WdeRender::cleanUp() {
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up Rendering Engine ==" << logger::endl;
		// Clean Up
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up Done ==" << logger::endl;
	}


	// On message from another module
	void WdeRender::onNotify(core::Event event) {
		logger::log() << "Notification received from Rendering engine : " << event.msg << logger::endl;
	}
}
