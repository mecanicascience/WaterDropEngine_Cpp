#pragma once

#include "../../../wde.hpp"
#include "../images/Image2D.hpp"
#include "../render/RenderAttachment.hpp"
#include "../render/RenderPassStructure.hpp"
#include "../images/ImageDepth.hpp"

namespace wde::render {
	/**
	 * Holds frame buffers for a render pass, one for each frame in the swapchain
	 */
	class FrameBuffers : public NonCopyable {
		public:
			// Constructors
			/**
			 * Create a frame buffer from a render pass
			 * @param attachments
			 * @param subpassesStructure
			 * @param inputAttachments
			 * @param renderPass
			 * @param depthStencil
			 */
			FrameBuffers(const std::vector<RenderAttachment>& attachments,
			             const std::vector<RenderSubPassStructure>& subpassesStructure,
			             const std::vector<uint32_t>& inputAttachments,
						 VkRenderPass& renderPass,
						 ImageDepth& depthStencil);
			~FrameBuffers() override;

			// Getters and setters
			VkFramebuffer& get(uint32_t index) { return _framebuffers[index]; }


		private:
			std::vector<VkFramebuffer> _framebuffers;
			std::vector<std::unique_ptr<Image2D>> _imageAttachments;
	};
}
