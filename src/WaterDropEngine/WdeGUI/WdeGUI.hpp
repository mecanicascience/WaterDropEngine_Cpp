#pragma once

#include <vulkan/vulkan_core.h>
#include "../WdeCore/WdeModule.hpp"
#include "../WdeCommon/WdeLogger/Logger.hpp"
#include "../WdeRenderingEngine/WdeRenderEngine.hpp"

#include "../../../lib/imgui/imgui.h"
#include "../../../lib/imgui/backends/imgui_impl_glfw.h"
#include "../../../lib/imgui/backends/imgui_impl_vulkan.h"

namespace wde::gui {
	class WdeGUI : public WdeModule::Module<WdeGUI> {
		// Register module
		inline static const int MODULE_REGISTERED = RegisterModule("gui", Stage::POST_RENDER);

		public:
			// Core module functions
			/** Initialize the module */
			void initialize() override;
			/** On engine ticks */
			void tick() override;
			/** Clean up the module */
			void cleanUp() override;

			// Core functions
			/** Setups the GUI (called by the GUISubrenderer) */
			void setup();
			/** Renders the GUI to ImGUI (called by the GUISubrenderer) */
			void render();


		private:
			/** Setup our custom color scheme */
			static void setCustomColorScheme();
	};
}
