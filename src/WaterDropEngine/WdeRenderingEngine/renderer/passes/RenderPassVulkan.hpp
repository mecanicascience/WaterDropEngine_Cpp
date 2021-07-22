#pragma once

#include <vulkan/vulkan_core.h>
#include <optional>
#include <vector>

#include "../descriptors/RenderSubpassDescription.hpp"

namespace wde::renderEngine {
	class RenderPass;

	/**
	 * A class that contains and handles a vulkan render pass described by a RenderPass
	 */
	class RenderPassVulkan {
		public:
			// Core instance
			/**
			 * Creates a new Vulkan render pass given the format
			 * @param pass
			 * @param swapchainFormat
			 */
			RenderPassVulkan(RenderPass &pass, VkFormat swapchainFormat);
			/** Clean up the render pass */
			void cleanUp();

			// Getters and setters
			operator const VkRenderPass &() const { return _renderPass; }


		private:
			/** Associated Vulkan render pass */
			VkRenderPass _renderPass = VK_NULL_HANDLE;
	};
}
