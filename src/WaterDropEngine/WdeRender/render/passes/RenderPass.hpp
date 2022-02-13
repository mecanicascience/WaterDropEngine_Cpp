#pragma once

#include <utility>

#include "../../../../wde.hpp"
#include "../../../WdeCommon/WdeUtils/Color.hpp"

namespace wde::render {
	/**
	 * Represents a render attachment
	 */
	class RenderAttachment {
		public:
			enum Type {
				SWAPCHAIN,  // layout : PRESENT_SRC_KHR
				DEPTH,      // layout : DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				IMAGE       // layout : COLOR_ATTACHMENT_OPTIMAL
			};


			/** Binding name of the attachment */
			std::string name;
			/** Description of the attachments */
			VkAttachmentDescription description {};

			/**
			 * Create a new render attachment
			 * @param name Binding name of the attachment
			 * @param type Type of the attachment
			 * @param format Format of it
			 */
			RenderAttachment(std::string  name, Type type, VkFormat format, const Color &clearColor = Color(0, 0, 0)) : name(std::move(name)) {
				description.samples = VK_SAMPLE_COUNT_1_BIT; // should match the swap chain images

				// Load operation
				description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear to black before new frame
				description.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // We need to store frame to see it

				// Same but to stencil data
				description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				// Attachment type
				/* Different values based on what we want to do with the images :
				 * VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
				 * VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
				 * VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation */
				description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // layout before render pass (VK_IMAGE_LAYOUT_UNDEFINED = don't care)

				// Set final layouts
				if (type == Type::SWAPCHAIN)
					description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // layout after render pass to auto transition to (need to be ready for presentation)
				else if (type == Type::DEPTH)
					description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				else if (type == Type::IMAGE)
					description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Create a colored image

				// Pass image format
				description.format = format;
			};
	};

	/**
	 * Represents a subpass of a render pass
	 */
	class RenderSubpass {
		public:
			RenderSubpass() {
				/*// CREATE COLOR ATTACHMENTS
				std::vector<VkAttachmentReference> subpassColorAttachments;
				auto bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // graphics or compute point
				for (const auto &attachmentBinding : subpassType.getAttachmentBindingIndices()) {
					auto attachment = _pass.getAttachment(attachmentBinding);
					if (!attachment)
						Logger::err("Failed to find the attachment of ID " + std::to_string(attachmentBinding) + ".", LoggerChannel::RENDERING_ENGINE);

					if (attachment->getType() == RenderPassAttachment::Type::Depth) {
						depthAttachment = attachment->getBindingIndex();
						continue;
					}

					VkAttachmentReference attachmentReference = {};
					attachmentReference.attachment = attachment->getBindingIndex(); // Shader can access attachment at specified index
					attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;  // Layout after sub-render pass to auto transition to
					subpassColorAttachments.emplace_back(attachmentReference);
				}



				VkSubpassDescription subpassDescription {};
				subpassDescription.pipelineBindPoint = bindPoint;

				// Set color attachments
				subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
				subpassDescription.pColorAttachments = colorAttachments.data();

				// Set optional depth attachment
				if (depthAttachment) {
					depthStencilAttachment.attachment = *depthAttachment;
					depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					subpassDescription.pDepthStencilAttachment = &depthStencilAttachment;
				}

				// Input attachments
				subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
				subpassDescription.pInputAttachments = inputAttachments.data();*/
			}
	};


	/**
	 * Represents a render pass
	 */
	class RenderPass {
		/**
		 * Create a new render pass
		 * @param attachments Attachments of the pass
		 * @param subpasses Render subpasses in order
		 */
		explicit RenderPass(std::vector<RenderAttachment>& attachments, std::vector<RenderSubpass>& subpasses);
	};
}
