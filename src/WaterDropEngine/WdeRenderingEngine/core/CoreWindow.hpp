#pragma once

#include <string>
#include <utility>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <vector>

#include "../../WdeCommon/WdeLogger/Logger.hpp"

namespace wde::renderEngine {
	class WdeRenderEngine;

	class CoreWindow {
		public:
			// Public variables
			bool sendInfoShouldResizeFrameBuffer = false;


			// Instantiation
			/**
		     * Creates the GLFW Window (only 1/app)
		     * @param width
		     * @param height
		     * @param windowName
		     */
			CoreWindow(WdeRenderEngine &renderEngine, int width, int height, std::string name) : renderEngine{renderEngine}, width{width}, height{height}, windowName{std::move(name)} {};

			/** Destruct the GLFW window (only 1/app) */
			~CoreWindow() = default;
			/** Clean up the window */
			void cleanUp();


			// Disables copy
			/** Create a copy of a window (Not allowed - Only one window in RenderEngine) */
			CoreWindow(const CoreWindow &) = delete;
			/** Compares two windows (Copy not allowed - Only one window in RenderEngine) */
			CoreWindow &operator=(const CoreWindow &) = delete;


			// Getters and setters
			GLFWwindow* getWindow() { return window; }
			int getWidth() { return width; };
			int getHeight() { return height; };


			// Main functions
			/** Initialise the window */
			void initialize();

			/** @return true if the window should close, false instead */
			bool shouldClose();

			/**
			 * Call this callback when the GLFW window has been resized
			 * @param window
			 * @param width
			 * @param height
			 */
			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);



			// Utils functions
			/**
			 * Choose the best swap surface format for renderer (color depth)
			 * @param availableFormats
			 * @return The best available format (preferably VK_FORMAT_B8G8R8A8_SRGB as a SRGB color space)
			 */
			static VkSurfaceFormatKHR chooseBestSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

			/**
			 * Choose the best presentation mode (frame renderer mode)
			 * @param availablePresentModes
			 * @return The best available presentation mode (preferably VK_PRESENT_MODE_MAILBOX_KHR)
			 */
			static VkPresentModeKHR chooseBestSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

			/**
			 * Choose the best swap extent mode (frame renderer resolution)
			 * @param *window
			 * @param capabilities
			 * @return The best swap extent mode (preferably the same as the pixel width and height of the image)
			 */
			static VkExtent2D chooseBestSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities);



		private:
			/** A reference to the render engine */
			WdeRenderEngine &renderEngine;

			/** GLFW Window width */
			int width;
			/** GLFW Window height */
			int height;
			/** GLFW displayed name of the Window */
			std::string windowName;


			/** The displayed GLFW window */
			GLFWwindow *window;
	};
}
