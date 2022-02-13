#pragma once

#include <utility>

#include "../../../../wde.hpp"
#include "RenderAttachment.hpp"
#include "RenderPassStructure.hpp"
#include "../../images/ImageDepth.hpp"
#include "../../buffers/FrameBuffers.hpp"

namespace wde::render {
	/**
	 * Represents a render pass
	 */
	class RenderPass {
		public:
			/**
			 * Create a new render pass given the arguments
			 * @param attachments Attachments of the pass
			 * @param subpassesStructure Render subpasses structures in order
			 */
			explicit RenderPass(std::vector<RenderAttachment>& attachments, std::vector<RenderSubPassStructure> subpassesStructure);
			~RenderPass();


			// Events
			void onWindowResized();


			// Render commands
			void start();
			static void end();

			static void startSubPass(uint32_t subPassIndex);
			static void endSubPass(uint32_t subPassIndex);


			// Getters and setters
			uint32_t getSubPassesCount() const { return _subpassesStructure.size(); }


		private:
			// Pass data
			std::vector<RenderAttachment> _attachments;
			std::vector<RenderSubPassStructure> _subpassesStructure;

			// Internal data
			/** The optional depth image */
			std::unique_ptr<ImageDepth> _depthStencil;
			/** Corresponding frame buffers */
			std::unique_ptr<FrameBuffers> _framebuffers;
			/** Clear values of the different attachments */
			std::vector<VkClearValue> _clearValues;

			// Vulkan data
			VkRenderPass _renderPass = VK_NULL_HANDLE;
	};
}
