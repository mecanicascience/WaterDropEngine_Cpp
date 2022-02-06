#include "WdeRender.hpp"

namespace wde {
	// Module commands
	WdeRender::WdeRender(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		std::cout << "Creating Render Engine." << std::endl;
	}

	void WdeRender::tick() {

	}

	void WdeRender::cleanUp() {
		std::cout << "Cleaning up render engine." << std::endl;
	}


	// On message from another module
	void WdeRender::onNotify(core::Event event) {
		std::cout << "Render received a message : " << event.msg << std::endl;
	}
}
