#pragma once

#include "../WdeCommon/WdeLogger/Logger.hpp"
#include "../WdeCommon/WdeError/WdeStatus.hpp"
#include "../WdeCommon/WdeUtils/Constants.hpp"
#include "../WdeCommon/WdeError/WdeException.hpp"
#include "core/CoreWindow.hpp"
#include "core/CoreInstance.hpp"
#include "rendering/pipelines/GraphicsDefaultPipeline.hpp"


namespace wde::renderEngine {
	class WdeRenderEngine {
		public:
			// Constructors
			WdeRenderEngine() = default;
			~WdeRenderEngine() = default;

			// Body functions
			/** Creates a new rendering engine instance */
			WdeStatus initialize();
			/** Called when a new tick on the gameLoop happend */
			WdeStatus tick();
			/** Cleans up the rendering engine instance */
			WdeStatus cleanUp();

			/** @return true if the application must stop */
			bool shouldEnd();

			/** @return the CoreInstance */
			CoreInstance& getInstance() { return instance; }
			/** @return the selected device */
			CoreDevice& getSelectedDevice() { return instance.getSelectedDevice(); }
			/**
			 * Set the rendering graphics pipelines for a specified device
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
			CoreWindow window {WIDTH, HEIGHT, Constants::APPLICATION_NAME + " Window"};
			CoreInstance instance {window};
	};
}
