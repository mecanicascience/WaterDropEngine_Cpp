#pragma once

#include "../WdeCommon/WdeLogger/Logger.hpp"
#include "../WdeCommon/WdeError/WdeStatus.hpp"
#include "../WdeCommon/WdeUtils/Constants.hpp"
#include "../WdeCommon/WdeError/WdeException.hpp"
#include "core/CoreWindow.hpp"
#include "core/CoreInstance.hpp"


namespace wde::renderEngine {
	class WdeRenderEngine {
		public:
			// Constructors
			WdeRenderEngine() = default;
			~WdeRenderEngine() = default;

			// Body functions
			/** Creates a new renderer engine instance */
			WdeStatus initialize();
			/** Called when a new tick on the gameLoop happend */
			WdeStatus tick();
			/** Draws the next frame to the screen */
			WdeStatus draw();
			/** Force the drawing of the next frame to the screen */
			WdeStatus forceDraw();
			/** Cleans up the renderer engine instance */
			WdeStatus cleanUp();

			/** @return true if the application must stop */
			bool shouldEnd();

			/** @return the CoreInstance */
			CoreInstance& getInstance() { return instance; }
			/** @return the selected device */
			CoreDevice& getSelectedDevice() { return instance.getSelectedDevice(); }
			/**
			 * Set the renderer graphics pipelines for a specified device
			 * @param device
			 * @param graphicsPipeline
			 */
			void setDeviceGraphicsPipeline(CoreDevice &device, GraphicsPipeline &graphicsPipeline, Renderer &renderer) {
				instance.getDeviceByID(device.getDeviceID()).setGraphicsPipeline(instance.getSurface(), graphicsPipeline, renderer);
			}



		private:
			/** Fixed initial Window width */
			static constexpr int WIDTH = 800;

			/** Fixed initial Window height */
			static constexpr int HEIGHT = 600;

			// Class variables
			CoreWindow window {*this, WIDTH, HEIGHT, Constants::APPLICATION_NAME + " Window"};
			CoreInstance instance {window};
	};
}
