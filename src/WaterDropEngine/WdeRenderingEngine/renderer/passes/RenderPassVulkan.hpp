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
			 * @param depthFormat
			 */
            explicit RenderPassVulkan(RenderPass &pass, VkFormat swapchainFormat, VkFormat depthFormat);
			/**
			 * Initialize the Vulkan render pass
			 * @param swapchainFormat
			 * @param depthFormat
			 */
			void initialize(VkFormat swapchainFormat, VkFormat depthFormat);
			/** Clean up the render pass */
			void cleanUp();

			// Getters and setters
			operator const VkRenderPass &() const { return _renderPass; }


		private:
			/** The reference to the parent render-pass */
			RenderPass &_pass;
			/** Associated Vulkan render pass */
			VkRenderPass _renderPass = VK_NULL_HANDLE;
	};
}
