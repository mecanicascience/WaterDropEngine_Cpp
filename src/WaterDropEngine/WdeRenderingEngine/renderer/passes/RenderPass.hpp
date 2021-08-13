#pragma once

#include <vector>

#include "../../../../wde.hpp"
#include "../descriptors/RenderPassAttachment.hpp"
#include "../descriptors/RenderSubpassType.hpp"
#include "SwapChain.hpp"
#include "RenderPassVulkan.hpp"
#include "../../buffers/Framebuffers.hpp"
#include "../../images/ImageDepth.hpp"

namespace wde::renderEngine {
	/**
	 * Represents a Vulkan render area
	 */
	class RenderArea {
		public:
			// Constructors
			explicit RenderArea(VkOffset2D offset = {}, VkExtent2D extent = {}) : _offset(offset), _extent(extent) {}

			bool operator==(const RenderArea &rhs) const {
				return _extent.width == rhs._extent.width && _extent.height == rhs._extent.height && _offset.x == rhs._offset.x && _offset.y == rhs._offset.y;
			}

			// Getters and setters
			const VkOffset2D& getOffset() const { return _offset; }
			const VkExtent2D& getExtent() const { return _extent; }

			void setOffset(int x, int y) { _offset = {x, y}; }
			void setExtent(uint32_t width, uint32_t height) { _extent = {width, height}; }


		private:
			VkOffset2D _offset {0, 0};
			VkExtent2D _extent {0, 0};
	};


	/**
	 * Represents a Vulkan render pass
	 */
	class RenderPass {
		public:
			// Core functions
			/**
			 * Creates a render pass
			 * @param attachments The attachments of the render pass
			 * @param sub-passes The sub-passes of the render pass
			 */
			explicit RenderPass(std::vector<RenderPassAttachment> attachments, std::vector<RenderSubpassType> subpasses)
				: RenderPass(std::move(attachments), std::move(subpasses), RenderArea {{0, 0}, {0, 0}}) {}

			/**
			 * Creates a render pass
			 * @param attachments The attachments of the render pass
			 * @param sub-passes The sub-passes of the render pass
			 */
			explicit RenderPass(std::vector<RenderPassAttachment> attachments, std::vector<RenderSubpassType> subpasses, RenderArea renderArea);

			/**
			 * Initialize the Render pass and binds it to a swapchain
			 * @param swapchain
			 */
			void initialize(SwapChain &swapchain);
			/** Updates the render pass (updates the view size) */
			void update();

			/** Cleanup the pass */
			void cleanUp();


			// Getters and setters
			const std::vector<RenderPassAttachment> &getAttachments() const { return _attachments; };
			const std::vector<RenderSubpassType>& getSubpasses() const { return _subpasses; }
			const std::vector<VkClearValue> &getClearValues() const { return _clearValues; }
			std::optional<RenderPassAttachment> getAttachment(const std::string &name) const;
			std::optional<RenderPassAttachment> getAttachment(uint32_t binding) const;
			uint32_t getAttachmentCount(uint32_t subpass) const { return _subpassAttachmentCount[subpass]; }
			uint32_t getAttachmentColorCount(uint32_t subpass) const { return _subpassAttachmentColorCount[subpass]; }
			const RenderPassVulkan *getRenderPass() const { return _renderPass.get(); }
			bool hasSwapchain() const { return _swapchainAttachment.has_value(); }
			VkFramebuffer& getActiveFramebuffer(uint32_t activeSwapchainImage);
			Framebuffers& getFrameBuffers() { return *_framebuffers; }
			RenderArea& getRenderArea() { return _renderArea; }
			ImageDepth& getDepthStencil() { return *_depthStencil; }


		private:
			// Class internal data
			/** The corresponding render pass */
			std::unique_ptr<RenderPassVulkan> _renderPass;
			/** The swapchain corresponding color frame buffers */
			std::unique_ptr<Framebuffers> _framebuffers;

			/** The optional associated depth attachment */
            std::optional<RenderPassAttachment> _depthAttachment;
            /** The optional depth image */
            std::unique_ptr<ImageDepth> _depthStencil;


			// Constructor data
			/** Render pass attachments */
			std::vector<RenderPassAttachment> _attachments;
			/** Sub-passes of the render pass */
			std::vector<RenderSubpassType> _subpasses;
			/** Number of attachments in the subpass */
			std::vector<uint32_t> _subpassAttachmentCount;
			/** Number of color attachments in the subpass */
			std::vector<uint32_t> _subpassAttachmentColorCount;
			/** The attachments that are used in subpass as inputs */
			std::vector<int> _bindingInputAttachments {};

			/** Use swapchain extent as render area if render area has a width and height of 0 */
			bool _useSwapchainExtent = false;
			/** The area that the render pass will write to */
			RenderArea _renderArea {{0, 0}, {0, 0}};


			/** Clear values of the different attachments */
			std::vector<VkClearValue> _clearValues;
			/** The optional attachment to the swapchain */
			std::optional<RenderPassAttachment> _swapchainAttachment;
	};
}
