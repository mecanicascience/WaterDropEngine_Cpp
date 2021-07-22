#include "WdeRenderEngine.hpp"

namespace wde::renderEngine {
	// Core functions
	void WdeRenderEngine::initialize() {
		Logger::debug("== Initializing Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);
		// Initialize the GLFW window
		_window.initialize();
		CoreInstance::get().setWindow(&_window);

		// Initialize the Vulkan instance, the surface, and the devices
		CoreInstance::get().initialize();
		Logger::debug("== Initialization Done ==", LoggerChannel::RENDERING_ENGINE);
	}


	void WdeRenderEngine::tick() {
		glfwPollEvents(); // Poll GLFW events (user interactions, close event, waiting during minimizing, ...)
		draw(); // Draw next frame to the screen
	}

	void WdeRenderEngine::draw() {
		// The selected device will draw the next frame to the screen
		Logger::debug("Drawing frame to the screen.", LoggerChannel::RENDERING_ENGINE);
		CoreInstance::get().getSelectedDevice().draw();
	}


	void WdeRenderEngine::cleanUp() {
		Logger::debug("== Cleaning up Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);
		CoreInstance::get().cleanUp();
		_window.cleanUp();
		Logger::debug("== Cleaning up Done ==", LoggerChannel::RENDERING_ENGINE);
	}




	// Helper functions
	bool WdeRenderEngine::shouldEnd() {
		return _window.shouldClose();
	}

	void WdeRenderEngine::waitForDevicesReady() {
		CoreInstance::get().waitForDevicesReady();
	}
}
