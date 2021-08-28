#include "Image.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
    Image::Image(VkImageType type, VkImageViewType viewType, uint32_t arrayLayers, VkFormat format, VkExtent3D extent, uint32_t mipLevels, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkImageAspectFlags imageAspect, uint32_t baseMipLevel, uint32_t baseArrayLayer, uint32_t layerCount, bool initialize)
        : _type(type), _viewType(viewType), _arrayLayers(arrayLayers), _format(format), _extent(extent), _mipLevels(mipLevels), _tiling(tiling), _usage(usage), _memoryProperties(memoryProperties), _imageAspect(imageAspect), _baseMipLevel(baseMipLevel), _baseArrayLayer(baseArrayLayer), _layerCount(layerCount) {
        if (format == VK_FORMAT_UNDEFINED)
            throw WdeException("The specified image format is undefined.", LoggerChannel::RENDERING_ENGINE);

        if (initialize) {
        	Logger::debug("Creating a Vulkan image.", LoggerChannel::RENDERING_ENGINE);
        	createImage();

        	Logger::debug("Creating a Vulkan image view.", LoggerChannel::RENDERING_ENGINE);
        	createImageView();
        }
    }

    Image::~Image() {
        WDE_PROFILE_FUNCTION();
        VkDevice &device = CoreInstance::get().getSelectedDevice().getDevice();

        // Cleanup image view
        vkDestroyImageView(device, _view, nullptr);

        // Cleanup image memory and image
        vkFreeMemory(device, _memory, nullptr);
        vkDestroyImage(device, _image, nullptr);
    }




    void Image::createImage() {
        WDE_PROFILE_FUNCTION();
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
        _layout = VK_IMAGE_LAYOUT_UNDEFINED;

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
        WDE_PROFILE_FUNCTION();
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
        Logger::warn("Didn't found required image format.", LoggerChannel::RENDERING_ENGINE);
        return VK_FORMAT_UNDEFINED;
    }

    void Image::takeScreenshot(const std::string &filePath) {
        Logger::debug("Saving image to " + filePath + ".", LoggerChannel::RENDERING_ENGINE);
        CoreDevice& device = CoreInstance::get().getSelectedDevice();
        VkImageLayout srcImageLayout = _layout; // Save actual image layout

        // == Checks blit support ==
        auto supportsBlit = true;
        VkFormatProperties formatProperties;

        // Check if the device supports blitting from optimal images
        vkGetPhysicalDeviceFormatProperties(device.getPhysicalDevice(), _format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
            Logger::warn("Device does not support blitting from optimal tiled images, using copy instead of blit.", LoggerChannel::RENDERING_ENGINE);
            supportsBlit = false;
        }

        // Check if the device supports blitting to linear images
        vkGetPhysicalDeviceFormatProperties(device.getPhysicalDevice(), VK_FORMAT_R8G8B8A8_UNORM, &formatProperties);

        if (!(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {
            Logger::warn("Device does not support blitting to linear tiled images, using copy instead of blit.", LoggerChannel::RENDERING_ENGINE);
            supportsBlit = false;
        }



        // == Create destination image ==
        Image dstImage { VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, 1, VK_FORMAT_R8G8B8A8_UNORM,
              {_extent.width, _extent.height, 1}, 1, VK_IMAGE_TILING_LINEAR,
             VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
             VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1, false };
        dstImage.createImage();



        // == Copy source image to destination ==
        CommandBuffer commandBuffer { true };

        // Transition destination image to transfer destination layout.
        insertImageMemoryBarrier(commandBuffer, dstImage.getImage(), 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);

        // Transition image from previous usage to transfer source layout
        insertImageMemoryBarrier(commandBuffer, _image, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT, srcImageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, _mipLevels, 1, _arrayLayers);

        // If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB)
        /*if (supportsBlit) {
			// Define the region to blit (we will blit the whole swapchain image)
			VkOffset3D blitSize;
			blitSize.x = (int32_t) _extent.width;
			blitSize.y = (int32_t) _extent.height;
			blitSize.z = 1;
			VkImageBlit imageBlitRegion{};
			imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlitRegion.srcSubresource.layerCount = 1;
			imageBlitRegion.srcOffsets[1] = blitSize;
			imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlitRegion.dstSubresource.layerCount = 1;
			imageBlitRegion.dstOffsets[1] = blitSize;

			// Issue the blit command
			vkCmdBlitImage(
				commandBuffer,
                _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				dstImage.getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&imageBlitRegion,
				VK_FILTER_NEAREST);
		}
		else {
			// Otherwise use image copy (requires us to manually flip components)
			VkImageCopy imageCopyRegion{};
			imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopyRegion.srcSubresource.layerCount = 1;
			imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopyRegion.dstSubresource.layerCount = 1;
			imageCopyRegion.extent.width = (int32_t) _extent.width;
			imageCopyRegion.extent.height = (int32_t) _extent.height;
			imageCopyRegion.extent.depth = 1;

			// Issue the copy command
			vkCmdCopyImage(
                commandBuffer,
				_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				dstImage.getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&imageCopyRegion);

            // Transition destination image to general layout, which is the required layout for mapping the image memory later on
            insertImageMemoryBarrier(commandBuffer, dstImage.getImage(), VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);

            // Transition back the image after the blit is done
            insertImageMemoryBarrier(commandBuffer, _image, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcImageLayout,
                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, _mipLevels, 1, _arrayLayers);

		    // Submit command buffer
		    commandBuffer.end();
		    commandBuffer.submit();
		    commandBuffer.waitForQueueIdle();
		}



		// == Save distant image to file ==
		// Get layout of the image (including row pitch).
        VkImageSubresource imageSubresource = {};
        imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresource.mipLevel = 0;
        imageSubresource.arrayLayer = 0;

        VkSubresourceLayout dstSubresourceLayout;
        vkGetImageSubresourceLayout(device.getDevice(), dstImage.getImage(), &imageSubresource, &dstSubresourceLayout);


        // Map image memory so we can start copying from it
        const char* data;
        vkMapMemory(device.getDevice(), dstImage.getMemory(), 0, VK_WHOLE_SIZE, 0, (void**)&data);
        data += dstSubresourceLayout.offset;

        std::ofstream file(filePath, std::ios::out | std::ios::binary);

        // ppm header
        file << "P6\n" << _extent.width << "\n" << _extent.height << "\n" << 255 << "\n";

        // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
        bool colorSwizzle = false;
        // Check if source is BGR
        if (!supportsBlit) {
            std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM };
			colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), _format) != formatsBGR.end());
            Logger::warn("Image as bitmap saving is not yet implemented without the blit feature.", LoggerChannel::RENDERING_ENGINE);
        }

        // ppm binary pixel data
        for (uint32_t y = 0; y < _extent.height; y++) {
            unsigned int *row = (unsigned int*)data;
            for (uint32_t x = 0; x < _extent.width; x++) {
                if (colorSwizzle) {
                    file.write((char*)row+2, 1);
                    file.write((char*)row+1, 1);
                    file.write((char*)row, 1);
                }
                else {
                    file.write((char*)row, 3);
                }
                row++;
            }
            data += dstSubresourceLayout.rowPitch;
        }
        file.close();

        Logger::debug("Image saved to disk.", LoggerChannel::RENDERING_ENGINE);

        // Clean up resources
        vkUnmapMemory(device.getDevice(), dstImage.getMemory());
        vkFreeMemory(device.getDevice(), dstImage.getMemory(), nullptr);
        vkDestroyImage(device.getDevice(), dstImage.getImage(), nullptr);*/
    }

    void Image::insertImageMemoryBarrier(CommandBuffer commandBuffer, VkImage &image, VkAccessFlags srcAccessMasks,
                 VkAccessFlags dstAccessMasks, VkImageLayout oldImageLayout,
                 VkImageLayout newImageLayout, VkPipelineStageFlagBits srcStageMask,
                 VkPipelineStageFlagBits dstStageMask, VkImageAspectFlagBits imageAspect,
                 int mipLevels, int baseMipLevel, int layerCount, int baseArrayLayer) {
        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = srcAccessMasks;
        imageMemoryBarrier.dstAccessMask = dstAccessMasks;
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange.aspectMask = imageAspect;
        imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
        imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
        imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
        imageMemoryBarrier.subresourceRange.layerCount = layerCount;

        // Transition layouts
        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    }
}
