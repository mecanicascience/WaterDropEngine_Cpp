#pragma once

#include "../../wde.hpp"
#include "core/CoreWindow.hpp"
#include "core/CoreInstance.hpp"


namespace wde::renderEngine {
	/**
	 * The unique main render engine class.
	 */
	class WdeRenderEngine : NonCopyable {
		public:
			// Instance
            explicit WdeRenderEngine() {
				// Creates static instance
				CoreInstance::get();
			}


			// Core functions
			/** Creates a new renderer engine instance */
			void initialize();
			/** Called when a new tick on the gameLoop happen */
			void tick();
			/** Draws the next frame to the screen */
			static void draw();
			/** Cleans up the renderer engine instance */
			void cleanUp();


			// Helper functions
			/** @return true if the application must stop */
			bool shouldEnd();
			/** Wait until every devices are done finishing their jobs */
			void waitForDevicesReady();



		private:
			/** Fixed initial Window width */
			static constexpr int WIDTH = 800;
			/** Fixed initial Window height */
			static constexpr int HEIGHT = 600;

			// Class variables
			CoreWindow _window {*this, WIDTH, HEIGHT, Constants::APPLICATION_NAME + " Window"};
	};
}
