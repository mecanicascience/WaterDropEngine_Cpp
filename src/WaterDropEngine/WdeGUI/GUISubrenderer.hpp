#pragma once

#include "../../../lib/imgui/backends/imgui_impl_vulkan.h"
#include "../../../lib/imgui/backends/imgui_impl_glfw.h"

#include "../WdeRenderingEngine/WdeRenderEngine.hpp"

using namespace wde::renderEngine;

namespace wde::gui {
	class GUISubrenderer : public Subrenderer {
		public:
			/** Creates the renderer given the stage */
			explicit GUISubrenderer(const RenderStage &stage);
			~GUISubrenderer();

			/** Records commands on the given command buffer */
			void render(CommandBuffer &commandBuffer) override;

		private:
			VkDescriptorPool _descriptorPool;
	};
}
