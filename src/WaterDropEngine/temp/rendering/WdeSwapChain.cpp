#include "WdeSwapChain.hpp"

namespace wde {
    WdeSwapChain::~WdeSwapChain() {
        // Destroy frame buffers
        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device.getDevice(), framebuffer, nullptr);
        }

        // Destroy render pass
        vkDestroyRenderPass(device.getDevice(), renderPass, nullptr);

        // Destroy images views
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device.getDevice(), imageView, nullptr);
        }

        // Destroy swap chain
        vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);
    }

    void WdeSwapChain::initialize() {
        // Create swap chain (stores to this->swapChain and to this->swapChainImages)
        createSwapChain();

        // Create a basic image view for every image in the swap chain
        createImageViews();

        // Tell Vulkan about frame-buffers used for rendering (we currently want 1 rendering pass that renders the full scene)
        // Render passes are responsible of passing data from one pipeline to another
        createRenderPasses();

        // Create the frame buffers
        createFrameBuffers();
    }


    void WdeSwapChain::createSwapChain() {
        VkPhysicalDevice physicalDevice = device.getPhysicalDevice();
        SwapChainSupportDetails swapChainSupport = device.querySwapChainSupport(physicalDevice);

        // Choose the best image data from what's available
        VkSurfaceFormatKHR surfaceFormat = wde::WdeWindow::chooseBestSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = wde::WdeWindow::chooseBestSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = window.chooseBestSwapExtent(swapChainSupport.capabilities);

        // Log output format
        std::cout << std::endl << "Selected a surface format of VkFormat(" << surfaceFormat.format << ") with a color space of VkColorSpaceKHR(" << surfaceFormat.colorSpace << ")." << std::endl;
        std::cout << "Selected image ratio of " << extent.width << "x" << extent.height << " px." << std::endl;

        // We choose to have as possible, the min image (+ 1 to avoid waiting for next image) in the swap chain
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        std::cout << "The swap-chain will support a count of " + std::to_string(imageCount) + " images." << std::endl;

        // Create struct
        VkSwapchainCreateInfoKHR createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = device.getSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT if post processing

        // Set queues and swap chain compatibles
        QueueFamilyIndices indices = device.findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        // Can specify images transformations (currentTransform = no transformation)
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE; // Clipping
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        // Create swap chain
        if (vkCreateSwapchainKHR(device.getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain.");
        }
        std::cout << "Created swap chain." << std::endl;

        // Say to store swap chain items to swapChainImages
        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void WdeSwapChain::recreateSwapChain() {
        // Handle minimization (wait)
        int width = 0, height = 0;
        glfwGetFramebufferSize(window.getWindow(), &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window.getWindow(), &width, &height);
            glfwWaitEvents();
        }

        // Wait until operations finished
        vkDeviceWaitIdle(device.getDevice());

        // Destroy last swap chain
        cleanUpSwapChain();

        // Create new swap chain
        createSwapChain();
        createImageViews();
        createRenderPasses();

        std::string pathVert = "res/shaders/simpleShader.vert.spv";
        std::string pathFrag = "res/shaders/simpleShader.frag.spv";
        pipeline->createGraphicsPipeline(pathVert, pathFrag);
        createFrameBuffers();
        renderer->createCommandBuffers();
    }

    void WdeSwapChain::cleanUpSwapChain() {
        // Destroy frame buffers
        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device.getDevice(), framebuffer, nullptr);
        }

        // Destroy command buffers
        renderer->clearCommandBuffers();

        // Destroy pipelines
        pipeline->destroyPipeline();

        // Destroy render pass
        vkDestroyRenderPass(device.getDevice(), renderPass, nullptr);

        // Destroy images views
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device.getDevice(), imageView, nullptr);
        }

        // Destroy swap chain
        vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);
    }



    void WdeSwapChain::createImageViews() {
        // Setup each image view for each frame
        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];

            // Select 2D textures
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;

            // Default color mapping
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            // Images used without mipmapping levels or multiple layers
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            // Create image view
            if (vkCreateImageView(device.getDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void WdeSwapChain::createRenderPasses() {
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
        // subpass.pInputAttachments (read from a shader)
        // subpass.pResolveAttachments (used for multisampling)
        // subpass.pDepthStencilAttachment (for depth and stencil data)
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
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        // Create render pass
        if (vkCreateRenderPass(device.getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass.");
        }
    }

    void WdeSwapChain::createFrameBuffers() {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        // Create frame buffers from image views
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageView attachments[] = { swapChainImageViews[i] };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass; // Render pass compatible with the frame buffer
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments; // VkImageVie objects bound to
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1; // Our swap chain are single images

            if (vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer.");
            }
        }
    }
}