#include "CoreWindow.hpp"
#include "../WdeRenderEngine.hpp"

namespace wde::renderEngine {
	// Core functions
	void CoreWindow::initialize() {
		WDE_PROFILE_FUNCTION();
		// Initialize the window
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not use OpenGL API (since Vulkan used)

		// Create the window
		_window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);

		// Add callbacks
		glfwSetWindowUserPointer(_window, this);
		glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
	}

	void CoreWindow::cleanUp() {
		WDE_PROFILE_FUNCTION();
		// Terminate the GLFW window
		glfwDestroyWindow(_window);
		glfwTerminate(); // terminates GLFW library

		// Clears window pointer
		_window = nullptr;
	}



	// Helper functions
	void CoreWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		// On new window size
		auto appWindow = reinterpret_cast<CoreWindow*>(glfwGetWindowUserPointer(window));
		appWindow->sendInfoShouldResizeFrameBuffer = true;
		appWindow->_width = width;
		appWindow->_height = height;

		// Send resize infos
		WdeRenderEngine::draw();
	}



	bool CoreWindow::shouldClose() {
		return glfwWindowShouldClose(_window);
	}
}
