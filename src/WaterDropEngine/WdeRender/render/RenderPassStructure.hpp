#pragma once

#include <utility>

#include "../../../wde.hpp"

namespace wde::render {
	// Pass command manager
	/**
	 * Represents a subpass (note : bindingIDs must be in numerical order)
	 */
	class RenderSubPassStructure {
		public:
			uint32_t _subpassID;
			std::vector<uint32_t> _readingAttachments;
			std::vector<uint32_t> _writingAttachments;
			VkPipelineStageFlags _srcStageMask;
			VkPipelineStageFlags _dstStageMask;
			VkPipelineStageFlags _srcAccessMask;
			VkPipelineStageFlags _dstAccessMask;

			/**
			 * Structure of a render subpass
			 * @param subpassID Binding ID
			 * @param readingAttachments Bindings IDs of the attachments that the subpass can read from
			 * @param writingAttachments Bindings IDs of the attachments that the subpass can write to
			 * @param srcStageMask Pipeline stage must have completed to start sub-pass (default : Allow to output color to memory, before fragment shading)
			 * @param dstStageMask Pipeline stage is waiting on the sub-pass (default :  Allow to output color to memory, before fragment shading)
			 * @param srcAccessMask Read memory accesses scopes of the sub-pass (default : No accesses)
			 * @param dstAccessMask Writes memory accesses scopes of the sub-pass (default : Writes accesses to color, resolve, or depth attachment)
			 */
			RenderSubPassStructure(uint32_t subpassID, std::vector<uint32_t>  readingAttachments, std::vector<uint32_t> writingAttachments = {},
			                       VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			                       VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			                       VkPipelineStageFlags srcAccessMask = VK_ACCESS_NONE_KHR,
			                       VkPipelineStageFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT) :
					_subpassID(subpassID), _readingAttachments(std::move(readingAttachments)), _writingAttachments(std::move(writingAttachments)),
					_srcStageMask(srcStageMask), _dstStageMask(dstStageMask),
					_srcAccessMask(srcAccessMask), _dstAccessMask(dstAccessMask) {}
	};

	/**
	 * Represents a render pass (note : bindingIDs must be in numerical order)
	 */
	class RenderPassStructure {
		public:
			std::vector<RenderSubPassStructure> _subPasses;
			uint32_t _passID;

			RenderPassStructure(uint32_t passID, std::vector<RenderSubPassStructure>  subPasses) : _passID(passID), _subPasses(std::move(subPasses)) {}
	};
}
