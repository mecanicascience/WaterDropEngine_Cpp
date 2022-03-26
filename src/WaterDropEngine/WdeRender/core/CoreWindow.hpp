#pragma once

#include "../../../wde.hpp"
#include "../../WdeCore/Structure/Subject.hpp"
#include <utility>

namespace wde::render {
	/**
	 * Holds an instance of the GLFW game window
	 */
	class CoreWindow : public NonCopyable {
		public:
			explicit CoreWindow(std::shared_ptr<core::Subject> moduleSubject, std::pair<int, int> windowSize, std::string windowName);
			~CoreWindow() override;


			// Getters and setters
			GLFWwindow* getWindow() { return _window; }


		private:
			/** GLFW CoreWindow size */
			std::pair<int, int> _windowSize;
			/** GLFW displayed name of the CoreWindow */
			std::string _windowName;

			/** The displayed GLFW window */
			GLFWwindow* _window = nullptr;
			/** Common modules discussion channel */
			std::shared_ptr<core::Subject> _moduleSubject;


			/**
			 * Callback when the window is resized
			 * @param window
			 * @param width
			 * @param height
			 */
			static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
	};
}
