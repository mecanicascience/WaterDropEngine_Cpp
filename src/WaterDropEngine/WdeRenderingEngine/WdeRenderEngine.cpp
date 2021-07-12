#include "WdeRenderEngine.hpp"

namespace wde::renderEngine {
	WdeStatus WdeRenderEngine::initialize() {
		Logger::debug("== Initializing Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);

		// Initialize the GLFW window
		window.initialize();

		// Initialize the Vulkan instance, the surface, and the devices
		instance.initialize(window);

		Logger::debug("== Initialization Done ==", LoggerChannel::RENDERING_ENGINE);

		return WdeStatus::WDE_SUCCESS;
	}


	WdeStatus WdeRenderEngine::tick() {
		glfwPollEvents(); // Poll GLFW events (user interactions, close event, ...)
		instance.getSelectedDevice().drawFrame(window);

		return WdeStatus::WDE_SUCCESS;
	}


	WdeStatus WdeRenderEngine::cleanUp() {
		Logger::debug("== Cleaning up Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);
		instance.cleanUp();
		window.cleanUp();
		Logger::debug("== Cleaning up Done ==", LoggerChannel::RENDERING_ENGINE);

		return WdeStatus::WDE_SUCCESS;
	}


	bool WdeRenderEngine::shouldEnd() {
		return window.shouldClose();
	}
}
