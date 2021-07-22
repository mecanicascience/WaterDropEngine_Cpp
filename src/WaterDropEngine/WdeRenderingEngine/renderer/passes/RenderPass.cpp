#include "RenderPass.hpp"
#include "../../core/CoreInstance.hpp"

namespace wde::renderEngine {
	RenderPass::RenderPass(std::vector<RenderPassAttachment> attachments, std::vector<RenderSubpassType> subpasses)
			: _attachments(std::move(attachments)), _subpasses(std::move(subpasses)) {
		// Stores attachments based on their types
		for (const auto &attachment : _attachments) {
			VkClearValue clearValue = {};

			// Switch attachment type and handle it
			switch(attachment.getType()) {
				case RenderPassAttachment::Type::Swapchain:
					clearValue.color = {{attachment.getClearColor()._r, attachment.getClearColor()._g, attachment.getClearColor()._b, attachment.getClearColor()._a}};
					_swapchainAttachment = attachment;
					break;

				case RenderPassAttachment::Type::Depth:
					// TODO Not implemented yet
					break;
			}

			// Add clear value to the stack
			_clearValues.emplace_back(clearValue);
		}
	}


	void RenderPass::initialize(SwapChain &swapChain) {
		// Creates associated render pass
		if (!_renderPass)
			_renderPass = std::make_unique<RenderPassVulkan>(*this, swapChain.getImageFormat());
		else
			_renderPass->initialize(swapChain.getImageFormat());


		// Create associated frame buffers
		_framebuffers = std::make_unique<Framebuffers>(*this, *_renderPass, swapChain );
	}

	void RenderPass::update() { }

	void RenderPass::cleanUp() {
		// Clean up frame buffers
		if (_framebuffers)
			_framebuffers->cleanUp();

		// Clean up render pass
		if (_renderPass)
			_renderPass->cleanUp();
	}




	// Getters
	std::optional<RenderPassAttachment> RenderPass::getAttachment(const std::string &name) const {
		auto it = std::find_if(_attachments.begin(), _attachments.end(), [name](const RenderPassAttachment &a) {
			return a.getUniqueName() == name;
		});

		if (it != _attachments.end())
			return *it;
		return std::nullopt;
	}

	std::optional<RenderPassAttachment> RenderPass::getAttachment(uint32_t binding) const {
		auto it = std::find_if(_attachments.begin(), _attachments.end(), [binding](const RenderPassAttachment &a) {
			return a.getBindingIndex() == binding;
		});

		if (it != _attachments.end())
			return *it;
		return std::nullopt;
	}

	VkFramebuffer& RenderPass::getActiveFramebuffer(uint32_t activeSwapchainImage) {
		if (activeSwapchainImage > _framebuffers->getFramebuffers().size())
			return _framebuffers->getFramebuffers().at(0);

		return _framebuffers->getFramebuffers().at(activeSwapchainImage);
	}
}
