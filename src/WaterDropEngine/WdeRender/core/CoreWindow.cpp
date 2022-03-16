#include "CoreWindow.hpp"

#include <utility>

namespace wde::render {
	CoreWindow::CoreWindow(std::shared_ptr<core::Subject> moduleSubject, std::pair<int, int> windowSize, std::string windowName)
			: _moduleSubject(std::move(moduleSubject)), _windowSize(std::move(windowSize)), _windowName(std::move(windowName)) {
		WDE_PROFILE_FUNCTION();

		// Setup glfw
		{
			WDE_PROFILE_FUNCTION();
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating GLFW window." << logger::endl;
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not use OpenGL API (since Vulkan used)
		}

		// Create window
		{
			WDE_PROFILE_FUNCTION();
			if (Config::IS_FULLSCREEN) {
				auto monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				_window = glfwCreateWindow(mode->width, mode->height, _windowName.c_str(), monitor, nullptr);
			}
			else {
				_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowName.c_str(), nullptr, nullptr);
			}
		}

		// Add callbacks
		{
			WDE_PROFILE_FUNCTION();
			glfwSetWindowUserPointer(_window, this);
			glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
		}
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
		appWindow->_moduleSubject->notify({LogChannel::RENDER, "WINDOW_RESIZED"});
	}
}
