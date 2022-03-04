#pragma once

#include "../../../wde.hpp"
#include "../../WdeCommon/WdeUtils/Color.hpp"

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


			/** Binding ID of the attachment */
			uint32_t bindingID;
			/** Description of the attachments */
			VkAttachmentDescription description {};
			/** Type of the attachment */
			Type type;
			/** Clear color of the attachment used at each new render pass */
			Color clearColor;

			/**
			 * Create a new render attachment
			 * @param bindingID Binding ID of the attachment
			 * @param name Name of the attachment
			 * @param type Type of the attachment (SWAPCHAIN, DEPTH or IMAGE)
			 * @param format Vulkan attachment format (default : VK_FORMAT_UNDEFINED)
			 * @param clearColor Clear color of the attachment at the beginning of each render pass (default : Black)
			 */
			RenderAttachment(uint32_t bindingID, const std::string& name, Type type, VkFormat format = VK_FORMAT_UNDEFINED, const Color& clearColor = Color(0, 0, 0))
					: bindingID(bindingID), type(type), clearColor(clearColor) {
				description.samples = VK_SAMPLE_COUNT_1_BIT;
				if (type == Type::IMAGE)
					description.samples = VK_SAMPLE_COUNT_64_BIT; // should match the swap chain images

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
				if (type == Type::SWAPCHAIN) {
					description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // layout after render pass to auto transition to (need to be ready for presentation)
					description.format = VK_FORMAT_UNDEFINED; // Format set in the RenderPass
				}
				else if (type == Type::DEPTH) {
					description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					description.format = VK_FORMAT_UNDEFINED; // Format set in the RenderPass
				}
				else if (type == Type::IMAGE) {
					description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Create a colored image
					description.format = format;
				}
			};
	};
}
