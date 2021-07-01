#include "WdeRenderer.hpp"

namespace wde {
    WdeRenderer::~WdeRenderer() {
        VkDevice devicePhys = device.getDevice();

        // Destroy semaphores and fences
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(devicePhys, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(devicePhys, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(devicePhys, inFlightFences[i], nullptr);
        }

        // Destroy command pool
        vkDestroyCommandPool(devicePhys, commandPool, nullptr);
    }


    void WdeRenderer::initialize() {
        // Create the command pool (allocate memory to buffers)
        createCommandPool();

        // Allocate command buffers
        createCommandBuffers();

        // Create semaphores for sync
        createSyncObjects();
    }


    void WdeRenderer::drawFrame() {
        VkDevice devicePhys = device.getDevice();

        // Wait for the current frame being rendered at the current frame index
        vkWaitForFences(devicePhys, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);


        // ==== Rendering process (operations should come in order, so using semaphores) ====
        // == Acquire image from swap chain to the semaphore and check if image not already being rendered ==
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(
                devicePhys,
                swapChain.getSwapChain(),
                UINT64_MAX, // don't set timeout before image becomes available
                imageAvailableSemaphores[currentFrame], // signaled when presentation engine finished using image
                VK_NULL_HANDLE,
                &imageIndex); // output index of swap chain image that became available (frame is at swapChainImages[imageIndex])
        if (result == VK_ERROR_OUT_OF_DATE_KHR) { // Need to be reset
            swapChain.recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image.");
        }

        // Wait if a previous frame is using this image (i.e. there is its fence to wait on)
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
            vkWaitForFences(devicePhys, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        // Mark the image as now being in use by this frame
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];



        // == Execute to command buffer with image as attachment in framebuffer ==
        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // Specify semaphore to wait for
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;

        // Stage of the pipeline to wait for the semaphore (wait at writing to color attachment, can already start vertex shader and so on)
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
        vkResetFences(devicePhys, 1, &inFlightFences[currentFrame]);

        // Test success and submit queue (triggers inFlightFences[currentFrame] when frame end submitting to presentation queue)
        if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer.");
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
        result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) { // Needs to be reset
            framebufferResized = false;
            swapChain.recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image.");
        }


        // Go to next frame
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }


    void WdeRenderer::clearCommandBuffers() {
        // Command buffers not already empty
        if (!commandBuffers.empty())
            vkFreeCommandBuffers(device.getDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }



    void WdeRenderer::createCommandPool() {
        // Get supported queues
        QueueFamilyIndices queueFamilyIndices = device.findQueueFamilies(device.getPhysicalDevice());

        // Create command pool
        VkCommandPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily; // Connect graphics queues and command buffer
        poolInfo.flags = 0; // Optional

        if (vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool.");
        }
    }

    void WdeRenderer::createCommandBuffers() {
        commandBuffers.resize(swapChain.getSwapChainFrameBuffers().size());

        // Create allocation infos about buffer allocation
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        // Primary (can be submit to queue but cannot be called from other command buffers) or
        // Secondary (cannot be submit directly but can be called from primary command buffers) command buffers
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        // Allocate
        if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers.");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            // === Starts command buffer recording ===
            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer.");
            }



            // === Starts a render pass ===
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = swapChain.getRenderPass(); // attached render pass
            renderPassInfo.framebuffer = swapChain.getSwapChainFrameBuffers()[i]; // attached frame buffer

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent(); // drawing size

            // Clear colors
            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f}; // black
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


            // === Binds to the graphics pipeline ===
            vkCmdBindPipeline(commandBuffers[i], // attached command buffers
                              VK_PIPELINE_BIND_POINT_GRAPHICS, // tell graphics or compute pipeline
                              pipeline.getGraphicsPipeline());


            // === Tell to draw the triangle ===
            vkCmdDraw(commandBuffers[i],
                      3, // 3 vertices to draw
                      1, // instanced rendering (1 if not doing that)
                      0, // Offset into vertex buffer
                      0); // Offset into instanced rendering


            // === End render pass ===
            vkCmdEndRenderPass(commandBuffers[i]);


            // Test if success
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer.");
            }
        }
    }

    void WdeRenderer::createSyncObjects() {
        // Resize semaphores and fences sizes
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChain.getSwapChainImages().size(), VK_NULL_HANDLE); // Every images in flight is Null at the start

        // Create structs
        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Fence are enabled in init (to launch the program on the first frame rendering)

        // Create semaphores and connect them
        VkDevice devicePhys = device.getDevice();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(devicePhys, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
             || vkCreateSemaphore(devicePhys, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
             || vkCreateFence(devicePhys, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create synchronization objects for the " + std::to_string(i) + "th frame.");
            }
        }
    }
}
