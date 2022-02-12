#include "CoreWindow.hpp"

namespace wde::render {
	CoreWindow::CoreWindow(std::shared_ptr<core::Subject> moduleSubject, std::pair<int, int> windowSize, std::string windowName)
			: _moduleSubject(std::move(moduleSubject)), _windowSize(std::move(windowSize)), _windowName(std::move(windowName)) {
		WDE_PROFILE_FUNCTION();

		// Setup glfw
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating GLFW window." << logger::endl;
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not use OpenGL API (since Vulkan used)

		// Create window
		_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowName.c_str(), nullptr, nullptr);

		// Add callbacks
		glfwSetWindowUserPointer(_window, this);
		glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
	}

	CoreWindow::~CoreWindow() {
		WDE_PROFILE_FUNCTION();

		// Kill the window
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Destroying window." << logger::endl;
		glfwDestroyWindow(_window);

		// Terminates GLFW library
		glfwTerminate();

		// Clears window pointer
		_window = nullptr;
	}



	// Helper functions
	void CoreWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		// On new window size
		auto appWindow = reinterpret_cast<CoreWindow*>(glfwGetWindowUserPointer(window));
		appWindow->_windowSize.first = width;
		appWindow->_windowSize.second = height;

		// TODO : Handle resize notification to the engine
		appWindow->_moduleSubject->notify({LogChannel::RENDER, "WINDOW_RESIZED"});
	}
}
