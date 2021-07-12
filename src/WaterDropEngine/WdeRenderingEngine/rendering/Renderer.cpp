#include "Renderer.hpp"

namespace wde::renderEngine {
	void Renderer::initialize(VkPhysicalDevice &physicalDevice, VkDevice &device, VkSurfaceKHR &surface, VkRenderPass &renderPass,
							  VkPipeline &graphicsPipeline, std::vector<VkFramebuffer> &swapChainFrameBuffers, VkExtent2D &swapChainExtent, std::vector<VkImage>& swapChainImages) {
		// Allocate command buffers
		createCommandBuffers(device, graphicsPipeline, swapChainFrameBuffers, swapChainExtent, renderPass);

		// Create semaphores for sync
		createSyncObjects(device, swapChainImages);
	}

	void Renderer::cleanUp(VkDevice &device) {
		// Destroy semaphores and fences
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		// Destroy command pool
		vkDestroyCommandPool(device, commandPool, nullptr);
	}

	void Renderer::cleanUpCommandBuffers(VkDevice &device) {
		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	}



	void Renderer::createRenderPasses(VkDevice &device, VkFormat &swapChainImageFormat, VkFormat depthFormat) {
		// == Depth pass ==
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = depthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Depth attachment references
		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



		// == Attachment description ==
		VkAttachmentDescription colorAttachment {};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // should match the swap chain images

		/* loadOp (what to do with data before rendering) :
		 * VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
		 * VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
		 * VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them */
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear to black before new frame

		/* storeOp (what to do after rendering) :
		 *  VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
		 *  VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation */
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // We need to store frame to see it

		// Same but to stencil data
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		/* Different values based on what we want to do with the images :
		 * VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
		 * VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
		 * VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation */
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // layout before render pass (VK_IMAGE_LAYOUT_UNDEFINED = don't care)
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // layout after render pass to auto transition to (need to be ready for presentation)


		// == Sub-passes ==
		VkAttachmentReference colorAttachmentRef {};
		colorAttachmentRef.attachment = 0; // Reference to attachment 0 in array (/!\ ref in shader as layout(location = 0) /!\)
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;  // layout after sub-render pass to auto transition to

		VkSubpassDescription subpass {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // graphics subpass (not a compute one)

		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		// subpass.pInputAttachments (read from a shader)
		// subpass.pResolveAttachments (used for multisampling)
		// subpass.pPreserveAttachments (not used by subpass, but data must be preserved)


		// == Render pass (reference subpasses to one render pass) ==
		// Wait that we have acquired the image before passing it to the render pass
		VkSubpassDependency dependency {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // indices of dependency subpass (external input)
		dependency.dstSubpass = 0; // indices of dependent subpass (0 = index of our first subpass)
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Wait for swap chain to read from image before accessing
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Wait for color attachment
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		// Render pass struct
		std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		// Create render pass
		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw WdeException("Failed to create render pass.", LoggerChannel::RENDERING_ENGINE);
		}
	}



	void Renderer::drawFrame(CoreWindow &window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, SwapChain &swapChain, VkQueue &graphicsQueue, VkQueue &presentQueue) {
		// Wait for the current frame being rendered at the current frame index
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);


		// ==== Rendering process (operations should come in order, so using semaphores) ====
		// == Acquire image from swap chain to the semaphore and check if image not already being rendered ==
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(
				device,
				swapChain.getSwapChain(),
				UINT64_MAX, // don't set timeout before image becomes available
				imageAvailableSemaphores[currentFrame], // signaled when presentation engine finished using image
				VK_NULL_HANDLE,
				&imageIndex); // output index of swap chain image that became available (frame is at swapChainImages[imageIndex])

		if (result == VK_ERROR_OUT_OF_DATE_KHR) { // Need to be reset
			shouldRecreateSwapChainBool = true;
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw WdeException("Failed to acquire swap chain image.", LoggerChannel::RENDERING_ENGINE);
		}

		// Wait if a previous frame is using this image (i.e. there is its fence to wait on)
		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		// Mark the image as now being in use by this frame
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];



		// == Execute to command buffer with image as attachment in framebuffer ==
		VkSubmitInfo submitInfo {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		// Specify semaphore to wait for
		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;

		// Stage of the pipelines to wait for the semaphore (wait at writing to color attachment, can already start vertex shader and so on)
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;

		// Command buffers to submit for execution
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		// Semaphore to signal once command buffer finished execution
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// Resets blocking infos
		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		// Test success and submit queue (triggers inFlightFences[currentFrame] when frame end submitting to presentation queue)
		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw WdeException("Failed to submit draw command buffer.", LoggerChannel::RENDERING_ENGINE);
		}



		// == Return the image to swap chain for presentation ==
		// Presentation infos
		VkPresentInfoKHR presentInfo {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores; // semaphore to wait for

		// The swap chain to present to
		VkSwapchainKHR swapChains[] = { swapChain.getSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex; // "replace image"
		presentInfo.pResults = nullptr; // Optional

		// Submits to presentation queue
		result = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.sendInfoShouldResizeFrameBuffer) { // Needs to be reset
			window.sendInfoShouldResizeFrameBuffer = false;
			shouldRecreateSwapChainBool = true;
		}
		else if (result != VK_SUCCESS) {
			throw WdeException("Failed to present swap chain image.", LoggerChannel::RENDERING_ENGINE);
		}


		// Go to next frame
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}





	void Renderer::createCommandPool(VkPhysicalDevice &physicalDevice, VkDevice &device, VkSurfaceKHR &surface) {
		// Get supported queues
		QueueFamilyIndices queueFamilyIndices = CoreUtils::findQueueFamilies(physicalDevice, surface);

		// Create command pool
		VkCommandPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily; // Connect graphics queues and command buffer
		poolInfo.flags = 0; // Optional

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw WdeException("Failed to create command pool.", LoggerChannel::RENDERING_ENGINE);
		}
	}

	void Renderer::createCommandBuffers(VkDevice &device, VkPipeline &graphicsPipeline, std::vector<VkFramebuffer> &swapChainFrameBuffers, VkExtent2D &swapChainExtent, VkRenderPass &renderPass) {
		commandBuffers.resize(swapChainFrameBuffers.size());

		// Create allocation infos about buffer allocation
		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		// Primary (can be submit to queue but cannot be called from other command buffers) or
		// Secondary (cannot be submit directly but can be called from primary command buffers) command buffers
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

		// Allocate
		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw WdeException("Failed to allocate command buffers.", LoggerChannel::RENDERING_ENGINE);
		}

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			// === Starts command buffer recording ===
			VkCommandBufferBeginInfo beginInfo {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw WdeException("Failed to begin recording command buffer.", LoggerChannel::RENDERING_ENGINE);
			}


			// Generate the render passes for this command buffer
			createRenderPasses(commandBuffers[i], graphicsPipeline, renderPass, swapChainFrameBuffers[i], swapChainExtent);


			// Test if success
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw WdeException("Failed to record command buffer.", LoggerChannel::RENDERING_ENGINE);
			}
		}
	}

	void Renderer::createSyncObjects(VkDevice &device, std::vector<VkImage>& swapChainImages) {
		// Resize semaphores and fences sizes
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE); // Every images in flight is Null at the start

		// Create structs
		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Fence are enabled in init (to launch the program on the first frame rendering)

		// Create semaphores and connect them
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
			    || vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
			    || vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
				throw WdeException("Failed to create synchronization objects for the " + std::to_string(i) + "th frame.", LoggerChannel::RENDERING_ENGINE);
			}
		}
	}
}
