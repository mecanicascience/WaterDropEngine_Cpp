#include "WdeRenderEngine.hpp"
#include "../WdeScene/WdeSceneManager.hpp"

namespace wde::renderEngine {
	// Core functions
	void WdeRenderEngine::initialize() {
		// Only one initialization possible
		static bool initialized = false;
		if (initialized)
			return;
		initialized = true;

        WDE_PROFILE_FUNCTION();
        Logger::debug("== Initializing Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);
		// Initialize the GLFW window
		_window.initialize();
		CoreInstance::get().setWindow(&_window);

		// Initialize the Vulkan instance, the surface, and the devices
		CoreInstance::get().initialize();
		Logger::debug("== Initialization Done ==", LoggerChannel::RENDERING_ENGINE);
	}


	void WdeRenderEngine::tick() {
        WDE_PROFILE_FUNCTION();
		glfwPollEvents(); // Poll GLFW events (user interactions, close event, waiting during minimizing, ...)
		draw(); // Draw next frame to the screen
	}

	void WdeRenderEngine::draw() {
        WDE_PROFILE_FUNCTION();
        if (!scene::WdeSceneManager::get().hasScene())
        	return;

		// The selected device will draw the next frame to the screen
		Logger::debug("Drawing frame to the screen.", LoggerChannel::RENDERING_ENGINE);
		CoreInstance::get().getSelectedDevice().draw();
	}


	void WdeRenderEngine::cleanUp() {
        WDE_PROFILE_FUNCTION();
        Logger::debug("== Cleaning up Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);
		CoreInstance::get().cleanUp();
		_window.cleanUp();
		Logger::debug("== Cleaning up Done ==", LoggerChannel::RENDERING_ENGINE);
	}
}
