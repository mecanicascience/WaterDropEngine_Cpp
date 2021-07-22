#pragma once

#include <string>
#include <utility>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <vector>

#include "../../../wde.hpp"

namespace wde::renderEngine {
	class WdeRenderEngine;

	/**
	 * Module created for managing a window
	 */
	class CoreWindow : public NonCopyable {
		public:
			bool sendInfoShouldResizeFrameBuffer = false;

			/**
		     * Creates the GLFW Window (only 1/app)
		     * @param width
		     * @param height
		     * @param windowName
		     */
			CoreWindow(WdeRenderEngine &renderEngine, int width, int height, std::string name)
				: _renderEngine{renderEngine}, _width{width}, _height{height}, _windowName{std::move(name)} {};


			// Core functions
			/** Initialise the window */
			void initialize();
			/** Clean up the window */
			void cleanUp();

			// Getters and setters
			GLFWwindow* getWindow() { return _window; }



			// Helper functions
			/** @return true if the window should close, false instead */
			bool shouldClose();



		private:
			/** A reference to the render engine */
			WdeRenderEngine &_renderEngine;

			/** GLFW Window width */
			int _width;
			/** GLFW Window height */
			int _height;
			/** GLFW displayed name of the Window */
			std::string _windowName;


			/** The displayed GLFW window */
			GLFWwindow *_window = nullptr;


			// Helper functions
			/**
			 * Call this callback when the GLFW window has been resized
			 * @param window
			 * @param width
			 * @param height
			 */
			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}
