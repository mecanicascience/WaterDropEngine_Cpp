#include "Framebuffers.hpp"
#include "../renderer/passes/RenderPass.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
	Framebuffers::Framebuffers(RenderPass &renderPass, RenderPassVulkan &renderPassVulkan, SwapChain &swapchain, ImageDepth &depthStencil) {
        WDE_PROFILE_FUNCTION();
	    // No image attachment to create with depth and swapchain attachments
		// ...

		// Creating one frame buffer for each image in the swapchain
		_framebuffers.resize(swapchain.getImageCount());
		for (uint32_t i = 0; i < swapchain.getImageCount(); i++) {
			// Creating framebuffer attachments
			std::vector<VkImageView> attachments;

			for (const auto &attachment : renderPass.getAttachments()) {
				switch (attachment.getType()) {
					case RenderPassAttachment::Type::Swapchain:
						attachments.emplace_back(swapchain.getImageViews().at(i));
						break;

					case RenderPassAttachment::Type::Depth:
                        attachments.emplace_back(depthStencil.getView());
						break;
				}
			}

			// Creates corresponding frame buffer
			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPassVulkan;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = swapchain.getExtent().width; // TODO
			framebufferCreateInfo.height = swapchain.getExtent().height; // TODO
			framebufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(CoreInstance::get().getSelectedDevice().getDevice(), &framebufferCreateInfo, nullptr, &_framebuffers[i]) != VK_SUCCESS)
				throw WdeException("Failed to create frame buffer.", LoggerChannel::RENDERING_ENGINE);
		}
	}

	void Framebuffers::cleanUp() {
        WDE_PROFILE_FUNCTION();
	    // Cleanup frame buffers
		auto &device = CoreInstance::get().getSelectedDevice().getDevice();
		for (auto framebuffer : _framebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
}
