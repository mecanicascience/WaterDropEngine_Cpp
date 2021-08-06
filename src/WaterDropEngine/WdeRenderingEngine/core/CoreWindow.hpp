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
            explicit CoreWindow(WdeRenderEngine &renderEngine, int width, int height, std::string name)
				: _renderEngine{renderEngine}, _width{width}, _height{height}, _windowName{std::move(name)} {};


			// Core functions
			/** Initialise the window */
			void initialize();
			/** Clean up the window */
			void cleanUp();

			// Getters and setters
			GLFWwindow* getWindow() { return _window; }
			const int getWidth() const { return _width; }
			const int getHeight() const { return _height; }



			// Helper functions
			/** @return true if the window should close, false instead */
			bool shouldClose();
			/** Closes the window and ends the program */
			void closeWindow() { _shouldClose = true; }



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
			/** True if the window should close */
			bool _shouldClose = false;


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
