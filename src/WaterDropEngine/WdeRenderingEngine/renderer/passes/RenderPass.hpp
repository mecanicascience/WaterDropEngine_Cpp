#pragma once

#include <vector>

#include "../../../../wde.hpp"
#include "../descriptors/RenderPassAttachment.hpp"
#include "../descriptors/RenderSubpassType.hpp"
#include "SwapChain.hpp"
#include "RenderPassVulkan.hpp"
#include "../../buffers/Framebuffers.hpp"

namespace wde::renderEngine {
	class RenderPass {
		public:
			// Core functions
			/**
			 * Creates a render pass
			 * @param attachments The attachments of the render pass
			 * @param sub-passes The sub-passes of the render pass
			 */
			explicit RenderPass(std::vector<RenderPassAttachment> attachments = {}, std::vector<RenderSubpassType> subpasses = {});

			/**
			 * Initialize the Render pass and binds it to a swapchain
			 * @param swapChain
			 */
			void initialize(SwapChain &swapChain);
			/** Updates the render pass (updates the view size) */
			void update();

			/** Cleanup the pass */
			void cleanUp();


			// Getters and setters
			const std::vector<RenderPassAttachment> &getAttachments() const { return _attachments; };
			const std::vector<RenderSubpassType>& getSubpasses() const { return _subpasses; }
			const std::vector<VkClearValue> &getClearValues() const { return _clearValues; }
			std::optional<RenderPassAttachment> getAttachment(const std::string &name) const;
			std::optional<RenderPassAttachment> getAttachment(uint32_t binding) const;
			const RenderPassVulkan *getRenderPass() const { return _renderPass.get(); }
			bool hasSwapchain() const { return _swapchainAttachment.has_value(); }
			VkFramebuffer& getActiveFramebuffer(uint32_t activeSwapchainImage);


		private:
			// Class internal data
			/** The corresponding render pass */
			std::unique_ptr<RenderPassVulkan> _renderPass;
			std::unique_ptr<Framebuffers> _framebuffers;


			// Constructor data
			/** Render pass attachments */
			std::vector<RenderPassAttachment> _attachments;
			/** Sub-passes of the render pass */
			std::vector<RenderSubpassType> _subpasses;


			/** Clear values of the different attachments */
			std::vector<VkClearValue> _clearValues;
			/** The optional attachment to the swapchain */
			std::optional<RenderPassAttachment> _swapchainAttachment;
	};
}
