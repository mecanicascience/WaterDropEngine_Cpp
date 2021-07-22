#pragma once

#include "../../../wde.hpp"
#include "../renderer/passes/SwapChain.hpp"
#include "../renderer/descriptors/RenderPassAttachment.hpp"
#include "../renderer/passes/RenderPassVulkan.hpp"

namespace wde::renderEngine {
	class RenderPass;

	class Framebuffers : NonCopyable {
		public:
			// Constructors
			/**
			 * Create a new frame buffer manager given the parameter
			 * @param renderPass The corresponding render pass object
			 * @param renderPassVulkan The frame buffers corresponding Vulkan render passes
			 * @param swapchain The frame buffers associated swapchain
			 */
			Framebuffers(RenderPass &renderPass, RenderPassVulkan &renderPassVulkan, SwapChain &swapchain);
			void cleanUp();


			// Getters and setters
			std::vector<VkFramebuffer>& getFramebuffers() { return _framebuffers; }


		private:
			std::vector<VkFramebuffer> _framebuffers;
	};
}
