#include "CoreWindow.hpp"
#include "../../WdeResourceManager/resources/textures/Texture2D.hpp"

namespace wde::render {
	CoreWindow::CoreWindow(std::shared_ptr<core::Subject> moduleSubject, std::pair<int, int> windowSize, std::string windowName)
			: _moduleSubject(std::move(moduleSubject)), _windowSize(std::move(windowSize)), _windowName(std::move(windowName)) {
		WDE_PROFILE_FUNCTION();

		// Setup glfw
		{
			WDE_PROFILE_SCOPE("wde::render::CoreWindow::CoreWindow::createWindowGLFW()");
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating GLFW window." << logger::endl;
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not use OpenGL API (since Vulkan used)
		}

		// Create window
		{
			WDE_PROFILE_SCOPE("wde::render::CoreWindow::CoreWindow::createWindow()");
			if (Config::IS_FULLSCREEN) {
				auto monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				_window = glfwCreateWindow(mode->width, mode->height, _windowName.c_str(), monitor, nullptr);
			}
			else {
				_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowName.c_str(), nullptr, nullptr);
			}
		}

		// Set icon
		{
			WDE_PROFILE_SCOPE("wde::render::CoreWindow::CoreWindow::loadIcons()");
			GLFWimage images[4];
			images[3].pixels = resource::Texture2D::getImagePixels("res/icon/logo_16x16.png", images[3].width, images[3].height, 4);
			images[2].pixels = resource::Texture2D::getImagePixels("res/icon/logo_32x32.png", images[2].width, images[2].height, 4);
			images[1].pixels = resource::Texture2D::getImagePixels("res/icon/logo_48x48.png", images[1].width, images[1].height, 4);
			images[0].pixels = resource::Texture2D::getImagePixels("res/icon/logo_256x256.png", images[0].width, images[0].height, 4);
			glfwSetWindowIcon(_window, 1, images);
			resource::Texture2D::freeImagePixels(images[0].pixels);
			resource::Texture2D::freeImagePixels(images[1].pixels);
			resource::Texture2D::freeImagePixels(images[2].pixels);
			resource::Texture2D::freeImagePixels(images[3].pixels);
		}

		// Add callbacks
		{
			WDE_PROFILE_SCOPE("wde::render::CoreWindow::CoreWindow::createCallbacks()");
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
