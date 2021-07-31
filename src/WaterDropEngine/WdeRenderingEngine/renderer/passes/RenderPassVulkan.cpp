#include "RenderPassVulkan.hpp"
#include "RenderPass.hpp"
#include "../../core/CoreInstance.hpp"

namespace wde::renderEngine {
	RenderPassVulkan::RenderPassVulkan(RenderPass &pass, VkFormat swapchainFormat, VkFormat depthFormat) : _pass(pass) {
		initialize(swapchainFormat, depthFormat);
	}

	void RenderPassVulkan::initialize(VkFormat swapchainFormat, VkFormat depthFormat) {
        WDE_PROFILE_FUNCTION();
		// Creates the render passes attachments description objects
		std::vector<VkAttachmentDescription> attachmentDescriptions;

		for (const auto &attachment : _pass.getAttachments()) {
			VkAttachmentDescription attachmentDescription {};
			attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT; // should match the swap chain images

			/* loadOp (what to do with data before renderer) :
			 * VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
			 * VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
			 * VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them */
			attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear to black before new frame
			/* storeOp (what to do after renderer) :
			 *  VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
			 *  VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the renderer operation */
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // We need to store frame to see it

			// Same but to stencil data
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			/* Different values based on what we want to do with the images :
			 * VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
			 * VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
			 * VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation */
			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // layout before render pass (VK_IMAGE_LAYOUT_UNDEFINED = don't care)

			switch (attachment.getType()) {
				case RenderPassAttachment::Type::Swapchain:
					attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // layout after render pass to auto transition to (need to be ready for presentation)
					attachmentDescription.format = swapchainFormat;
					break;


				case RenderPassAttachment::Type::Depth:
                    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    attachmentDescription.format = depthFormat;
					break;
			}

			// Pushes attachment
			attachmentDescriptions.emplace_back(attachmentDescription);
		}



		// Create sub-passes informations
		std::vector<std::unique_ptr<RenderSubpassDescription>> subpasses;
		std::vector<VkSubpassDependency> dependencies;
		for (const auto &subpassType : _pass.getSubpasses()) {
			// == Sub-pass attachments references ==
			std::vector<VkAttachmentReference> subpassColorAttachments;
			std::optional<uint32_t> depthAttachment;

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
			// Set sub-pass attachment description
			subpasses.emplace_back(std::make_unique<RenderSubpassDescription>(bindPoint, subpassColorAttachments, depthAttachment));



			// == Sub-pass dependencies ==
			VkSubpassDependency subpassDependency = {};

			// Sub-pass sources and destinations (src : previous sub-pass,  dst : current sub-pass)
			if (subpassType.getBindingIndex() == 0) { // First sub-pass u0
				subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				subpassDependency.dstSubpass = subpassType.getBindingIndex();
			}
			else if (subpassType.getBindingIndex() <= _pass.getSubpasses().size() - 1) { // Sub-pass u : u0 < u < uN
				subpassDependency.srcSubpass = subpassType.getBindingIndex() - 1;
				subpassDependency.dstSubpass = subpassType.getBindingIndex();
			}
			else { // Last sub-pass uN
				subpassDependency.srcSubpass = subpassType.getBindingIndex() - 1;
				subpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			}


			// Sub-pass stage and resource access instructions
			subpassDependency.srcStageMask = subpassType.getSrcStageMask();
			subpassDependency.dstStageMask = subpassType.getDstStageMask();
			subpassDependency.srcAccessMask = subpassType.getSrcAccessMask();
			subpassDependency.dstAccessMask = subpassType.getDstAccessMask();
			subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Add sub-pass dependencies
			dependencies.emplace_back(subpassDependency);
		}

		// Setup sub-pass description vector
		std::vector<VkSubpassDescription> subpassDescriptions;
		subpassDescriptions.reserve(subpasses.size());
		for (const auto &subpass : subpasses) {
			subpassDescriptions.emplace_back(subpass->getSubpassDescription());
		}

		// Creates the render pass
		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassCreateInfo.pDependencies = dependencies.data();

		if(vkCreateRenderPass(CoreInstance::get().getSelectedDevice().getDevice(), &renderPassCreateInfo, nullptr, &_renderPass) != VK_SUCCESS)
			throw WdeException("Failed to create render pass.", LoggerChannel::RENDERING_ENGINE);
		Logger::debug("Created render pass.", LoggerChannel::RENDERING_ENGINE);
	}


	void RenderPassVulkan::cleanUp() {
        WDE_PROFILE_FUNCTION();
		// Destroy render pass
		vkDestroyRenderPass(CoreInstance::get().getSelectedDevice().getDevice(), _renderPass, nullptr);
	}
}
