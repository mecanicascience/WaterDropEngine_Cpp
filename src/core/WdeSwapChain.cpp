#include "WdeSwapChain.hpp"

namespace wde {
    WdeSwapChain::WdeSwapChain(WdeInstanceDevice &device, WdeWindow &window) : device{device}, window{window} { }

    WdeSwapChain::~WdeSwapChain() {
        // Destroy swap chain
        vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);

        // Destroy images
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device.getDevice(), imageView, nullptr);
        }
    }

    void WdeSwapChain::initialize() {
        // Create swap chain (stores to this->swapChain and to this->swapChainImages)
        createSwapChain();

        // Create a basic image view for every image in the swap chain
        createImageViews();
    }

    void WdeSwapChain::createSwapChain() {
        VkPhysicalDevice physicalDevice = device.getPhysicalDevice();
        SwapChainSupportDetails swapChainSupport = device.querySwapChainSupport(physicalDevice);

        // Choose the best image data from what's available
        VkSurfaceFormatKHR surfaceFormat = window.chooseBestSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = window.chooseBestSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = window.chooseBestSwapExtent(swapChainSupport.capabilities);

        // Log output format
        std::cout << std::endl << "Selected a surface format of VkFormat(" << surfaceFormat.format << ") with a color space of VkColorSpaceKHR(" << surfaceFormat.colorSpace << ")." << std::endl;
        std::cout << "Selected image ratio of " << extent.width << "x" << extent.height << " px." << std::endl;

        // We choose to have as possible, the min image (+ 1 for safety) in the swap chain
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        // Create struct
        VkSwapchainCreateInfoKHR createInfo{};
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

        // Say to store swap chain items to swapChainImages
        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
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
}