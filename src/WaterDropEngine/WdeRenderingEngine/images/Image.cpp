#include "Image.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
    Image::Image(VkImageType type, VkImageViewType viewType, uint32_t arrayLayers, VkFormat format, VkExtent3D extent, uint32_t mipLevels, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkImageAspectFlags imageAspect, uint32_t baseMipLevel, uint32_t baseArrayLayer, uint32_t layerCount)
        : _type(type), _viewType(viewType), _arrayLayers(arrayLayers), _format(format), _extent(extent), _mipLevels(mipLevels), _tiling(tiling), _usage(usage), _memoryProperties(memoryProperties), _imageAspect(imageAspect), _baseMipLevel(baseMipLevel), _baseArrayLayer(baseArrayLayer), _layerCount(layerCount) {
        if (format == VK_FORMAT_UNDEFINED)
            throw WdeException("The specified image format is undefined.", LoggerChannel::RENDERING_ENGINE);

        Logger::debug("Creating a Vulkan image.", LoggerChannel::RENDERING_ENGINE);
        createImage();

        Logger::debug("Creating a Vulkan image view.", LoggerChannel::RENDERING_ENGINE);
        createImageView();
    }

    Image::~Image() {
        VkDevice &device = CoreInstance::get().getSelectedDevice().getDevice();

        // Cleanup image view
        vkDestroyImageView(device, _view, nullptr);

        // Cleanup image memory and image
        vkFreeMemory(device, _memory, nullptr);
        vkDestroyImage(device, _image, nullptr);
    }




    void Image::createImage() {
        // Create image
        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.flags = _arrayLayers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
        imageCreateInfo.imageType = _type;
        imageCreateInfo.format = _format;
        imageCreateInfo.extent = _extent;
        imageCreateInfo.mipLevels = _mipLevels;
        imageCreateInfo.arrayLayers = _arrayLayers;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = _tiling;
        imageCreateInfo.usage = _usage;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkDevice &device = CoreInstance::get().getSelectedDevice().getDevice();
        if (vkCreateImage(device, &imageCreateInfo, nullptr, &_image) != VK_SUCCESS)
            throw WdeException("Failed to create image.", LoggerChannel::RENDERING_ENGINE);

        // Create image memory
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, _image, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = BufferUtils::findMemoryType(
                CoreInstance::get().getSelectedDevice().getPhysicalDevice(), memoryRequirements.memoryTypeBits, _memoryProperties);
        if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &_memory) != VK_SUCCESS)
            throw WdeException("Failed to allocate image memory.", LoggerChannel::RENDERING_ENGINE);

        // Binding image memory to image
        if (vkBindImageMemory(device, _image, _memory, 0) != VK_SUCCESS)
            throw WdeException("Failed to bind image memory to image.", LoggerChannel::RENDERING_ENGINE);
    }

    void Image::createImageView() {
        // Create image view
        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _image;
        viewInfo.viewType = _viewType;
        viewInfo.format = _format;
        viewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
        viewInfo.subresourceRange.aspectMask = _imageAspect;
        viewInfo.subresourceRange.baseMipLevel = _baseMipLevel;
        viewInfo.subresourceRange.levelCount = _mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = _baseArrayLayer;
        viewInfo.subresourceRange.layerCount = _layerCount;

        if (vkCreateImageView(CoreInstance::get().getSelectedDevice().getDevice(), &viewInfo, nullptr, &_view) != VK_SUCCESS)
            throw WdeException("Failed to create texture image view.", LoggerChannel::RENDERING_ENGINE);
    }



    VkFormat Image::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (const auto &format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(CoreInstance::get().getSelectedDevice().getPhysicalDevice(), format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
                return format;
            if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
                return format;
        }

        // Format not found in candidates
        return VK_FORMAT_UNDEFINED;
    }
}
