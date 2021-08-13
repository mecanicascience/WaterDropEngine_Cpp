#pragma once

#include "../../../../wde.hpp"

namespace wde::renderEngine {
	/**
	 * Describes a specific subpass
	 */
	class RenderSubpassDescription {
		public:
			explicit RenderSubpassDescription(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference> colorAttachments, const std::optional<uint32_t> &depthAttachment,
											  std::vector<VkAttachmentReference> inputAttachments)
					: _colorAttachments(std::move(colorAttachments)), _inputAttachments(std::move(inputAttachments)) {
            	// Graphics or compute
				_subpassDescription.pipelineBindPoint = bindPoint;

				// Set color attachments
				_subpassDescription.colorAttachmentCount = static_cast<uint32_t>(_colorAttachments.size());
				_subpassDescription.pColorAttachments = _colorAttachments.data();

				// Set optional depth attachment
				if (depthAttachment) {
					_depthStencilAttachment.attachment = *depthAttachment;
					_depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					_subpassDescription.pDepthStencilAttachment = &_depthStencilAttachment;
				}

				// Input attachments
				_subpassDescription.inputAttachmentCount = static_cast<uint32_t>(_inputAttachments.size());
				_subpassDescription.pInputAttachments = _inputAttachments.data();
			}

			const VkSubpassDescription &getSubpassDescription() const { return _subpassDescription; }


		private:
			VkSubpassDescription _subpassDescription = {};
			std::vector<VkAttachmentReference> _colorAttachments {};
			std::vector<VkAttachmentReference> _inputAttachments {};
			VkAttachmentReference _depthStencilAttachment = {};
	};
}
