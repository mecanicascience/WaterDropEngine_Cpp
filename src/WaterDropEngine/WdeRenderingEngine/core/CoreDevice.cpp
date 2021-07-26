#include "CoreDevice.hpp"
#include "CoreInstance.hpp"


namespace wde::renderEngine {
	// Core functions
	void CoreDevice::initialize() {
        WDE_PROFILE_FUNCTION();
		// Setup physical device, device, graphics queue, presentation queue
		Logger::debug("Binding physical device.", LoggerChannel::RENDERING_ENGINE);
		CoreDeviceHelper::setupPhysicalDevice(_deviceID, _physicalDevice);
		Logger::debug("Binding logical device and queues.", LoggerChannel::RENDERING_ENGINE);
		CoreDeviceHelper::setupLogicalDeviceAndQueues(_physicalDevice, _device, _graphicsQueue, _presentQueue, _deviceExtensions);

		// Create swapchain
		Logger::debug("= Initializing swapchain of device " + std::to_string(_deviceID) + " =", LoggerChannel::RENDERING_ENGINE);
		_swapchain.initialize();
	}

	void CoreDevice::draw() {
        WDE_PROFILE_FUNCTION();
		// Recreate the swapChain if needed (like if a user resized the window)
		if (_shouldRecreateSwapchain) {
			CoreInstance::get().recreateSwapChain();
			_shouldRecreateSwapchain = false;
		}
		Logger::debug("Drawing next frame to the screen.", LoggerChannel::RENDERING_ENGINE);

		CoreInstance &instance = CoreInstance::get();
		std::size_t &currentFrame = instance.getCurrentFrame();

		// Update every render passes
		Logger::debug("Updating every render pass.", LoggerChannel::RENDERING_ENGINE);
		for (auto &renderPass : instance.getRenderer()->getRenderPasses()) {
			renderPass->update();
		}

		// Acquire image from the swap chain
		Logger::debug("Acquiring swapchain next image.", LoggerChannel::RENDERING_ENGINE);
		VkResult result = _swapchain.aquireNextImage(instance.getImagesInFlightFences()[currentFrame], instance.getImagesAvailableSemaphores()[currentFrame]);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) { // Need to be reset
			_shouldRecreateSwapchain = true;
			return;
		}

		// Update command buffers on each render passes
		RenderStage stage;
		for (auto &renderPass : instance.getRenderer()->getRenderPasses()) {
			// Starts render pass
			Logger::debug("= Starting render pass =", LoggerChannel::RENDERING_ENGINE);
			if (!startRenderPass(*renderPass))
				return;

			// Render sub passes
			auto &commandBuffer = instance.getCommandBuffers()[_swapchain.getActiveImageIndex()];
			for (const auto &subpass : renderPass->getSubpasses()) {
				Logger::debug("Rendering sub-pass.", LoggerChannel::RENDERING_ENGINE);
				// Acquire ID
				stage.second = subpass.getBindingIndex();

				// Renders sub-render pipelines
				instance.getRenderer()->getSubRenderersManager().renderStage(stage, *commandBuffer);

				// If the current sub-pass is different from the previous one, go to the next sub-pass
				if (subpass.getBindingIndex() != renderPass->getSubpasses().back().getBindingIndex())
					vkCmdNextSubpass(*commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
			}

			// Ends render pass
			Logger::debug("= Ending render pass =", LoggerChannel::RENDERING_ENGINE);
			endRenderPass(*renderPass);
			stage.first++;
		}
	}

	void CoreDevice::cleanUp() {
        WDE_PROFILE_FUNCTION();
		// Swapchain
		_swapchain.cleanUp();

		// Destroy device
		vkDestroyDevice(_device, nullptr);
	}



	// Helper functions
	bool CoreDevice::startRenderPass(RenderPass &renderPass) {
        WDE_PROFILE_FUNCTION();
		auto &commandBuffer = CoreInstance::get().getCommandBuffers()[_swapchain.getActiveImageIndex()];

		// Begin command buffer recording
		if (!commandBuffer->isRunning())
			commandBuffer->begin();

		// Rendering area
		VkRect2D renderArea = {};
		renderArea.offset = { 0, 0 }; // TODO
		renderArea.extent = { _swapchain.getExtent().width, _swapchain.getExtent().height }; // TODO

		// Associated viewport
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(renderArea.extent.width);
		viewport.height = static_cast<float>(renderArea.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(*commandBuffer, 0, 1, &viewport);

		// Viewport scissor
		VkRect2D scissor = {};
		scissor.offset = renderArea.offset;
		scissor.extent = renderArea.extent;
		vkCmdSetScissor(*commandBuffer, 0, 1, &scissor);

		// Begin render pass
		auto clearValues = renderPass.getClearValues();
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = *renderPass.getRenderPass();  // attached render pass
		renderPassBeginInfo.framebuffer = renderPass.getActiveFramebuffer(_swapchain.getActiveImageIndex());  // attached frame buffer
		renderPassBeginInfo.renderArea = renderArea;
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();
		vkCmdBeginRenderPass(*commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		return true;
	}

	void CoreDevice::endRenderPass(RenderPass &renderPass) {
        WDE_PROFILE_FUNCTION();
		CoreInstance &instance = CoreInstance::get();
		std::size_t &currentFrame = instance.getCurrentFrame();
		auto &commandBuffer = CoreInstance::get().getCommandBuffers()[_swapchain.getActiveImageIndex()];

		// Ends the render pass
		vkCmdEndRenderPass(*commandBuffer);

		// If the render pass doesn't have a swapchain, don't present the image to the corresponding swapchain
		if (!renderPass.hasSwapchain())
			return;

		// Ends the command buffer to submit it to the presentation queue
		commandBuffer->end();
		commandBuffer->submit(instance.getImagesInFlightFences()[currentFrame],
							  instance.getImagesAvailableSemaphores()[currentFrame],
							  instance.getImagesRenderFinishedSemaphores()[currentFrame]);

		// Present the presentation queue to the swapchain
		auto presentResult = _swapchain.presentToQueue(_presentQueue, instance.getImagesRenderFinishedSemaphores()[currentFrame]);
		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || CoreInstance::get().getCoreWindow().sendInfoShouldResizeFrameBuffer) {
			CoreInstance::get().getCoreWindow().sendInfoShouldResizeFrameBuffer = false;
			_shouldRecreateSwapchain = true;
		}
		else if (presentResult != VK_SUCCESS)
			throw WdeException("Failed to present swap chain image.", LoggerChannel::RENDERING_ENGINE);

		// Update current frame ID
		currentFrame = (currentFrame + 1) % instance.getMaxFramesInFlight();
	}
}
