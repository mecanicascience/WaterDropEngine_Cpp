#include "FrameBuffers.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	FrameBuffers::FrameBuffers(const std::vector<RenderAttachment>& attachments, const std::vector<RenderSubPassStructure>& subpassesStructure,
	                           const std::vector<uint32_t>& inputAttachments, VkRenderPass& renderPass, ImageDepth& depthStencil) {
		WDE_PROFILE_FUNCTION();
		auto& deviceEn = WaterDropEngine::get().getRender().getInstance().getDevice();
		auto& device = deviceEn.getDevice();
		auto& swapchain = WaterDropEngine::get().getRender().getInstance().getSwapchain();

		// Create image attachments
		for (const auto &attachment : attachments) {
			switch (attachment.type) {
				case RenderAttachment::Type::SWAPCHAIN:
				case RenderAttachment::Type::DEPTH:
					_imageAttachments.emplace_back(nullptr);
					break;

				case RenderAttachment::Type::IMAGE:
					int usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
					if (std::find(inputAttachments.begin(), inputAttachments.end(), attachment.bindingID) != inputAttachments.end())
						usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;  // Used as an input in a subpass

					// Create attachment
					_imageAttachments.emplace_back(std::make_unique<Image2D>(attachment.description.format, swapchain.getExtent(), usage, deviceEn.getMaxUsableSampleCount()));
					break;
			}
		}


		// Creating one frame buffer for each image in the swapchain
		_framebuffers.resize(swapchain.getImageCount());
		for (uint32_t i = 0; i < swapchain.getImageCount(); i++) {
			// Creating framebuffer attachments
			std::vector<VkImageView> attach;

			for (const auto &attachment : attachments) {
				switch (attachment.type) {
					case RenderAttachment::Type::SWAPCHAIN:
						attach.emplace_back(swapchain.getImageViews().at(i));
						break;

					case RenderAttachment::Type::DEPTH:
						attach.emplace_back(depthStencil.getView());
						break;

					case RenderAttachment::Type::IMAGE:
						attach.emplace_back((_imageAttachments[attachment.bindingID].get())->getView());
						break;
				}
			}

			// Creates corresponding frame buffer
			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attach.size());
			framebufferCreateInfo.pAttachments = attach.data();
			framebufferCreateInfo.width = swapchain.getExtent().width;
			framebufferCreateInfo.height = swapchain.getExtent().height;
			framebufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &_framebuffers[i]) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create frame buffer.");
		}
	}


	FrameBuffers::~FrameBuffers() {
		WDE_PROFILE_FUNCTION();
		auto &device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		// Cleanup frame buffers
		for (auto framebuffer : _framebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
}
