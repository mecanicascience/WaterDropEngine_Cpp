#pragma once

namespace wde::renderEngine {
	/**
	 * Describes a specific subpass
	 */
	class RenderSubpassDescription {
		public:
            explicit RenderSubpassDescription(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference> colorAttachments, const std::optional<uint32_t> &depthAttachment)
					: _colorAttachments(std::move(colorAttachments)) {
				_subpassDescription.pipelineBindPoint = bindPoint;
				_subpassDescription.colorAttachmentCount = static_cast<uint32_t>(_colorAttachments.size());
				_subpassDescription.pColorAttachments = _colorAttachments.data();

				if (depthAttachment) {
					_depthStencilAttachment.attachment = *depthAttachment;
					_depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					_subpassDescription.pDepthStencilAttachment = &_depthStencilAttachment;
				}
			}

			const VkSubpassDescription &getSubpassDescription() const { return _subpassDescription; }


		private:
			VkSubpassDescription _subpassDescription = {};
			std::vector<VkAttachmentReference> _colorAttachments {};
			VkAttachmentReference _depthStencilAttachment = {};
	};
}
