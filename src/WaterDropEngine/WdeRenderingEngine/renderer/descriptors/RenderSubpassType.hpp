#pragma once

namespace wde::renderEngine {
	/**
	 * Class describing a sub-pass in a render pass
	 */
	class RenderSubpassType {
		public:
			/**
			 * Create a sub-pass in the corresponding render pass
			 * @param bindingIndex The index of the sub-pass
			 * @param attachmentBindingIndices The IDs of the attachments in the render pass that the sub-pass can access to
			 * @param srcStageMask Pipeline stage must have completed to start sub-pass (default : Allow to output color to memory, before fragment shading)
			 * @param dstStageMask Pipeline stage is waiting on the sub-pass (default :  Allow to output color to memory, before fragment shading)
			 * @param srcAccessMask Read memory accesses scopes of the sub-pass (default : No accesses)
			 * @param dstAccessMask Writes memory accesses scopes of the sub-pass (default : Writes accesses to color, resolve, or depth attachment)
			 */
			RenderSubpassType(uint32_t bindingIndex, std::vector<uint32_t> attachmentBindingIndices,
							  VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
							  VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
							  VkPipelineStageFlags srcAccessMask = VK_ACCESS_NONE_KHR,
							  VkPipelineStageFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT):
				_bindingIndex(bindingIndex), _attachmentBindingIndices(attachmentBindingIndices), _srcStageMask(srcStageMask), _dstStageMask(dstStageMask),
				_srcAccessMask(srcAccessMask), _dstAccessMask(dstAccessMask) {}

			// Getters and setters
			uint32_t getBindingIndex() const { return _bindingIndex; }
			const std::vector<uint32_t> getAttachmentBindingIndices() const { return _attachmentBindingIndices; }
			VkPipelineStageFlags getSrcStageMask() const { return _srcStageMask; }
			VkPipelineStageFlags getDstStageMask() const { return _dstStageMask; }
			VkPipelineStageFlags getSrcAccessMask() const { return _srcAccessMask; }
			VkPipelineStageFlags getDstAccessMask() const { return _dstAccessMask; }

		private:
			uint32_t _bindingIndex {};
			std::vector<uint32_t> _attachmentBindingIndices {};
			VkPipelineStageFlags _srcStageMask;
			VkPipelineStageFlags _dstStageMask;
			VkPipelineStageFlags _srcAccessMask;
			VkPipelineStageFlags _dstAccessMask;
	};
}
