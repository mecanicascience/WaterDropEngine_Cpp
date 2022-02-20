#include "RenderPass.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	RenderPass::RenderPass(std::vector<RenderAttachment>& attachments, std::vector<RenderSubPassStructure> subpassesStructure)
			: _attachments(attachments), _subpassesStructure(std::move(subpassesStructure)) {
		CoreInstance &renderInstance = WaterDropEngine::get().getRender().getInstance();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating render pass." << logger::endl;


		// ==== Create subpass descriptions ====
		// Get depth attachment
		int depthAttachmentID = -1;
		for (auto &att: _attachments) {
			if (att.type == RenderAttachment::DEPTH) {
				depthAttachmentID = static_cast<int>(att.bindingID);
				break;
			}
		}

		std::vector<VkSubpassDescription> subpassDescriptions;
		std::vector<std::vector<VkAttachmentReference>> colorAttachments;
		std::vector<std::vector<VkAttachmentReference>> inputAttachments;
		VkAttachmentReference depthAttachmentRef {};
		{
			WDE_PROFILE_FUNCTION();
			int i = 0;
			for (auto &subpass: _subpassesStructure) {
				colorAttachments.emplace_back();
				inputAttachments.emplace_back();

				VkSubpassDescription subpassDescription {};
				subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics point


				// Set color attachments
				for (auto &att: subpass._writingAttachments) {
					if (_attachments[att].type == RenderAttachment::DEPTH)
						continue;

					VkAttachmentReference attachmentReference = {};
					attachmentReference.attachment = att; // Shader can access attachment at specified index
					attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;  // Layout after sub-render pass to auto transition to
					colorAttachments[i].push_back(attachmentReference);
				}
				subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments[i].size());
				subpassDescription.pColorAttachments = colorAttachments[i].data();
				_attachmentsColorCount.push_back(static_cast<int>(colorAttachments[i].size()));


				// Set optional depth attachment
				bool isDepthAttachment = false;
				for (auto &att: subpass._writingAttachments) {
					if (_attachments[att].type == RenderAttachment::DEPTH) {
						isDepthAttachment = true;
						break;
					}
				}
				if (depthAttachmentID != -1 && isDepthAttachment) {
					depthAttachmentRef.attachment = depthAttachmentID;
					depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
				}


				// Input attachments
				for (auto &att: subpass._readingAttachments) {
					if (_attachments[att].type == RenderAttachment::DEPTH)
						throw WdeException(LogChannel::RENDER, "The engine doesn't currently support depth texture as inputs in render passes.");

					VkAttachmentReference attachmentReference = {};
					attachmentReference.attachment = att; // Shader can access attachment at specified index
					attachmentReference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;  // Layout after sub-render pass to auto transition to
					inputAttachments[i].emplace_back(attachmentReference);
				}
				subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments[i].size());
				subpassDescription.pInputAttachments = inputAttachments[i].data();

				subpassDescriptions.push_back(subpassDescription);
				i++;
			}
		}


		// ==== Handle attachments ====
		std::vector<VkAttachmentDescription> attachmentDescriptions;
		{
			WDE_PROFILE_FUNCTION();
			// Create stencils and buffers
			if (depthAttachmentID != -1)
				_depthStencil = std::make_unique<ImageDepth>(
						renderInstance.getSwapchain().getExtent(),
						VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);

			// Set descriptions
			for (auto &att: _attachments) {
				VkClearValue clearValue = {};

				// Set format if depth or swapchain image
				if (att.type == RenderAttachment::DEPTH) {
					att.description.format = _depthStencil ? _depthStencil->getFormat() : VK_FORMAT_UNDEFINED;
					clearValue.depthStencil = {1.0f, 0};
				}
				else if (att.type == RenderAttachment::SWAPCHAIN) {
					att.description.format = renderInstance.getSwapchain().getImageFormat();
					clearValue.color = {{att.clearColor._r, att.clearColor._g, att.clearColor._b, att.clearColor._a}};
				}
				else {
					clearValue.color = {{att.clearColor._r, att.clearColor._g, att.clearColor._b, att.clearColor._a}};
				}

				attachmentDescriptions.push_back(att.description);
				_clearValues.push_back(clearValue);
			}
		}


		// ==== Create render pass dependencies ====
		std::vector<VkSubpassDependency> dependencies;
		{
			WDE_PROFILE_FUNCTION();
			int counter = 0;
			for (auto &subpass: _subpassesStructure) {
				VkSubpassDependency subpassDependency = {};

				// Sub-pass sources and destinations (src : previous sub-pass,  dst : current sub-pass)
				if (counter == 0) { // First sub-pass u0
					subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
					subpassDependency.dstSubpass = counter;
				} else if (counter < _subpassesStructure.size()) { // Sub-pass u : u0 < u < uN
					subpassDependency.srcSubpass = counter - 1;
					subpassDependency.dstSubpass = counter;
				} else { // Last sub-pass uN
					subpassDependency.srcSubpass = counter - 1;
					subpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				}

				// Sub-pass stage and resource access instructions
				subpassDependency.srcStageMask = subpass._srcStageMask;
				subpassDependency.dstStageMask = subpass._dstStageMask;
				subpassDependency.srcAccessMask = subpass._srcAccessMask;
				subpassDependency.dstAccessMask = subpass._dstAccessMask;
				subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
				dependencies.push_back(subpassDependency);

				counter++;
			}
		}


		// ==== Creates the render pass ====
		{
			WDE_PROFILE_FUNCTION();
			VkRenderPassCreateInfo renderPassCreateInfo = {};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
			renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
			renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
			renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
			renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderPassCreateInfo.pDependencies = dependencies.data();

			if(vkCreateRenderPass(renderInstance.getDevice().getDevice(), &renderPassCreateInfo, nullptr, &_renderPass) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create render pass.");
		}

		// ==== Create the associated frame buffers ====
		{
			WDE_PROFILE_FUNCTION();
			// Create list of attachments input bindings id
			std::vector<uint32_t> bindingInputIDs {};
			for (const auto &subpass : _subpassesStructure) {
				auto subpassBindingInputIndices = subpass._readingAttachments;
				for (auto& index : subpassBindingInputIndices)
					bindingInputIDs.push_back(index);
			}

			// Create framebuffers
			_framebuffers = std::make_unique<FrameBuffers>(attachments, _subpassesStructure, bindingInputIDs, _renderPass, *_depthStencil);
		}
	}

	RenderPass::~RenderPass() {
		WDE_PROFILE_FUNCTION();

		// Destroy render pass
		vkDestroyRenderPass(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _renderPass, nullptr);
	}




	// Handle render passes
	void RenderPass::start() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "= Starting render pass =" << logger::endl;
		auto &commandBuffer = WaterDropEngine::get().getRender().getInstance().getCommandBuffers()[WaterDropEngine::get().getRender().getInstance().getSwapchain().getActiveImageIndex()];

		// Rendering area
		VkRect2D renderArea = {};
		renderArea.offset = { 0, 0 };
		renderArea.extent = {
				WaterDropEngine::get().getRender().getInstance().getSwapchain().getExtent().width,
				WaterDropEngine::get().getRender().getInstance().getSwapchain().getExtent().height
		};

		// Associated viewport (flips image around the horizontal axis)
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(renderArea.extent.height);
		viewport.width  =  static_cast<float>(renderArea.extent.width);
		viewport.height = -static_cast<float>(renderArea.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(*commandBuffer, 0, 1, &viewport);

		// Viewport scissor
		VkRect2D scissor = {};
		scissor.offset = renderArea.offset;
		scissor.extent = renderArea.extent;
		vkCmdSetScissor(*commandBuffer, 0, 1, &scissor);

		// Begin render pass
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = _renderPass;  // attached render pass
		renderPassBeginInfo.framebuffer = _framebuffers->get(WaterDropEngine::get().getRender().getInstance().getSwapchain().getActiveImageIndex());  // attached frame buffer
		renderPassBeginInfo.renderArea = renderArea;
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(_clearValues.size());
		renderPassBeginInfo.pClearValues = _clearValues.data();
		vkCmdBeginRenderPass(*commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void RenderPass::end() {
		WDE_PROFILE_FUNCTION();
		auto &commandBuffer = WaterDropEngine::get().getRender().getInstance().getCommandBuffers()[WaterDropEngine::get().getRender().getInstance().getSwapchain().getActiveImageIndex()];
		vkCmdEndRenderPass(*commandBuffer);

		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "= Ending render pass =" << logger::endl;
	}

	void RenderPass::startSubPass(uint32_t subPassIndex) {
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Starting subpass " << subPassIndex << "." << logger::endl;
		WDE_PROFILE_FUNCTION();
		if (subPassIndex == 0)
			return;

		auto &commandBuffer = WaterDropEngine::get().getRender().getInstance().getCommandBuffers()[WaterDropEngine::get().getRender().getInstance().getSwapchain().getActiveImageIndex()];
		vkCmdNextSubpass(*commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
	}

	void RenderPass::endSubPass(uint32_t subPassIndex) {
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Ending subpass " << subPassIndex << "." << logger::endl;
		WDE_PROFILE_FUNCTION();
	}
}
