#include "WdeRender.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::render {
	// Module commands
	WdeRender::WdeRender(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Initializing Rendering Engine ==" << logger::endl;

		// Create window
		_window = std::make_unique<CoreWindow>(_moduleSubject, std::pair<int, int>{Config::WIDTH, Config::HEIGHT}, Config::APPLICATION_NAME.c_str());

		// Create vulkan instance
		_coreInstance = std::make_unique<CoreInstance>(*_window);
	}

	void WdeRender::start() {
		WDE_PROFILE_FUNCTION();

		// Start instance
		_coreInstance->start();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Initialization Done ==" << logger::endl;
	}

	void WdeRender::tick() {
		WDE_PROFILE_FUNCTION();

		// Check if should reset elements
		if (_windowResized) {
			_coreInstance->onWindowResized();
			WaterDropEngine::get().getInstance().onWindowResizedInstance();
			_windowResized = false;
		}

		// Check if window should close
		if (glfwWindowShouldClose(_window->getWindow()))
			_moduleSubject->notify({LogChannel::RENDER, "WINDOW_SHOULD_CLOSE"});
	}

	void WdeRender::cleanUp() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Cleaning Up Rendering Engine ==" << logger::endl;

		// Clean Up
		_coreInstance->cleanUp();
		_window.reset();

		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "== Cleaning Up Done ==" << logger::endl;
	}


	// On message from another module
	void WdeRender::onNotify(const core::Event& event) {
		// Check if window should close
		if (event.channel == LogChannel::RENDER && event.name == "WINDOW_SHOULD_CLOSE")
			_shouldRun = false;

		// Check if window resized
		if (event.channel == LogChannel::RENDER && event.name == "WINDOW_RESIZED")
			_windowResized = true;
	}
}
