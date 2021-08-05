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
			/** Initialize the module */
			void initialize() override;
			/** On engine ticks */
			void tick() override;
			/** Clean up the module */
			void cleanUp() override;
	};
}
