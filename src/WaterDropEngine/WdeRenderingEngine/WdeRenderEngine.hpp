#pragma once

#include "../WdeCore/WdeModule.hpp"
#include "../../wde.hpp"
#include "core/CoreWindow.hpp"
#include "core/CoreInstance.hpp"


namespace wde::renderEngine {
	/**
	 * The unique main render engine class.
	 */
	class WdeRenderEngine : public WdeModule::Module<WdeRenderEngine> {
		// Register module
		inline static const int MODULE_REGISTERED = RegisterModule("renderEngine", Stage::RENDER);

		public:
			// Core functions
			/** Creates a new renderer engine instance */
			void initialize() override;
			/** Called when a new tick on the gameLoop happen */
			void tick() override;
			/** Cleans up the renderer engine instance */
			void cleanUp() override;

			/** Draws the next frame to the screen */
			static void draw();


			// Getters and setters
			static CoreInstance& get() { return CoreInstance::get(); }



		private:
			/** Fixed initial Window width */
			static constexpr int WIDTH = 800;
			/** Fixed initial Window height */
			static constexpr int HEIGHT = 600;

			// Class variables
			CoreWindow _window {*this, WIDTH, HEIGHT, Constants::APPLICATION_NAME};
	};
}
