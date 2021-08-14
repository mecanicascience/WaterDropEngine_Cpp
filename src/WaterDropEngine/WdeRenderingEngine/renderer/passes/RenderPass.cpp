#include "RenderPass.hpp"
#include "../../core/CoreInstance.hpp"

namespace wde::renderEngine {
	RenderPass::RenderPass(std::vector<RenderPassAttachment> attachments, std::vector<RenderSubpassType> subpasses, RenderArea renderArea)
		: _attachments(std::move(attachments)), _subpasses(std::move(subpasses)), _subpassAttachmentCount(_subpasses.size()),
	      _subpassAttachmentColorCount(_subpasses.size()), _renderArea(renderArea) {
        WDE_PROFILE_FUNCTION();
        // If render area has a width and height of 0, it will be bound to the dimensions of the swapchain
        if (renderArea.getExtent().width == 0 && renderArea.getExtent().height == 0)
        	_useSwapchainExtent = true;

		// Stores attachments based on their types
		for (const auto &attachment : _attachments) {
			VkClearValue clearValue = {};

			// Switch attachment type and handle it
			switch(attachment.getType()) {
				case RenderPassAttachment::Type::Swapchain:
					clearValue.color = {{attachment.getClearColor()._r, attachment.getClearColor()._g, attachment.getClearColor()._b, attachment.getClearColor()._a}};
					_swapchainAttachment = attachment;
					for (const auto &subpass : _subpasses) {
						_subpassAttachmentCount[subpass.getBindingIndex()]++;

						if ( // If the subpass contains the actual swapchain binding
							auto subpassBindingIndices = subpass.getAttachmentBindingIndices();
							std::find(subpassBindingIndices.begin(), subpassBindingIndices.end(), attachment.getBindingIndex()) != subpassBindingIndices.end()
						) {
							_subpassAttachmentColorCount[subpass.getBindingIndex()]++;
						}
					}
					break;

				case RenderPassAttachment::Type::Depth:
					clearValue.depthStencil = {1.0f, 0};
					_depthAttachment = attachment;
					break;

				case RenderPassAttachment::Type::Image:
					clearValue.color = {{attachment.getClearColor()._r, attachment.getClearColor()._g, attachment.getClearColor()._b, attachment.getClearColor()._a}};

					// Create images at each subpasses
					for (const auto &subpass : _subpasses) {
						if ( // If the subpass contains the actual loop image attachment, create the appropriate image
							auto subpassBindingIndices = subpass.getAttachmentBindingIndices();
							std::find(subpassBindingIndices.begin(), subpassBindingIndices.end(), attachment.getBindingIndex()) != subpassBindingIndices.end()
						) {
							if ( // Add a color attachment if it's not an input index
								auto subpassInputBindingIndices = subpass.getInputAttachmentBindingIndices();
								std::find(subpassInputBindingIndices.begin(), subpassInputBindingIndices.end(), attachment.getBindingIndex()) != subpassInputBindingIndices.end()
							) { }
							else {
								_subpassAttachmentColorCount[subpass.getBindingIndex()]++;
							}

							// Add an attachment to the subpass
							_subpassAttachmentCount[subpass.getBindingIndex()]++;
						}
					}
					break;
			}

			for (const auto &subpass : _subpasses) {
				auto subpassBindingInputIndices = subpass.getInputAttachmentBindingIndices();
				for (auto& index : subpassBindingInputIndices)
					_bindingInputAttachments.push_back(index);
			}

			// Add clear value to the stack
			_clearValues.emplace_back(clearValue);
		}
	}


	void RenderPass::initialize(SwapChain &swapchain) {
        WDE_PROFILE_FUNCTION();
		VkFormat swapchainFormat = swapchain.getImageFormat();

		if (_useSwapchainExtent) // Map render area to swapchain area
			_renderArea.setExtent(swapchain.getExtent().width, swapchain.getExtent().height);

		// Create depth stencil
		if (_depthAttachment)
			_depthStencil = std::make_unique<ImageDepth>(_renderArea.getExtent(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
        VkFormat depthFormat = _depthStencil ? _depthStencil->getFormat() : VK_FORMAT_UNDEFINED;

		// Creates associated render pass
		if (!_renderPass)
			_renderPass = std::make_unique<RenderPassVulkan>(*this, swapchainFormat, depthFormat);
		else
			_renderPass->initialize(swapchainFormat, depthFormat);

		// Create associated frame buffers
		_framebuffers = std::make_unique<Framebuffers>(_renderArea.getExtent(), *this, *_renderPass, swapchain, *_depthStencil, _bindingInputAttachments);
	}

	void RenderPass::update() {
        WDE_PROFILE_FUNCTION();

        if (_useSwapchainExtent) {
        	// Map render area to swapchain area
        	_renderArea.setExtent(
                CoreInstance::get().getSelectedDevice().getSwapChain().getExtent().width,
                CoreInstance::get().getSelectedDevice().getSwapChain().getExtent().height
            );
        }
	}

	void RenderPass::cleanUp() {
        WDE_PROFILE_FUNCTION();
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
