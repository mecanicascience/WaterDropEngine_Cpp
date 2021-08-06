#pragma once

#include <vulkan/vulkan_core.h>

#include "../../wde.hpp"
#include "../WdeCore/WdeModule.hpp"
#include "../WdeCommon/WdeLogger/Logger.hpp"
#include "../WdeRenderingEngine/WdeRenderEngine.hpp"
#include "../WdeScene/WdeSceneManager.hpp"

#include "GUITheme.hpp"
#include "GUILogger.hpp"
#include "GUIBar.hpp"


namespace wde::gui {
	class WdeGUI : public WdeModule::Module<WdeGUI> {
		// Register module
		inline static const int MODULE_REGISTERED = RegisterModule("gui", Stage::POST_RENDER);

		public:
			// Core module functions
			void initialize() override {};
			void tick()       override {};
			void cleanUp()    override {};


			// Core functions
			/** Setup the GUI (called by the GUISubrenderer) */
			void setup();
			/** Create the GUI components */
			void createElements();
			/** Renders the GUI to ImGUI (called by the GUISubrenderer) */
			void render();

			/** Resets the GUI */
			void reset() { _shouldResetElements = true; }


			// Getters and setters
			GUILogger& getLogger() { return _logger; }

		private:
			// Core parameters
			/** Main root dockspace ID */
			const std::string DOCKSPACE_ROOT_ID = "Main Dockspace Window";
			/** True if the GUI should be reset during next render frame */
			bool _shouldResetElements = false;

			// Gui classes
			GUIBar _guiBar {};
			GUILogger _logger {};
	};
}
