#pragma once

#include "../../../lib/imgui/backends/imgui_impl_vulkan.h"
#include "../../../lib/imgui/backends/imgui_impl_glfw.h"

#include "../WdeRenderingEngine/WdeRenderEngine.hpp"
#include "WdeGUI.hpp"

using namespace wde::renderEngine;

namespace wde::gui {
	class GUISubrenderer : public Subrenderer {
		public:
			/**
			 * Create the main Gui renderer
			 * @param stage
			 */
			explicit GUISubrenderer(const RenderStage &stage);
			/** Free descriptor pool and ImGui instance */
			~GUISubrenderer() override;

			/**
			 * Renders the GUI to the command buffer
			 * @param commandBuffer
			 */
			void render(CommandBuffer &commandBuffer) override;


		private:
			VkDescriptorPool _descriptorPool;
	};
}
