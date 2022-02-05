#include "WdeGUI.hpp"

namespace wde {
	// Module commands
	WdeGUI::WdeGUI(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		std::cout << "Creating GUI engine." << std::endl;
	}

	void WdeGUI::tick() {

	}

	void WdeGUI::cleanUp() {
		std::cout << "Cleaning up GUI." << std::endl;
	}


	// On message from another module
	void WdeGUI::onNotify(core::Event event) {
		std::cout << "GUI received a message : " << event.msg << std::endl;
	}
}
