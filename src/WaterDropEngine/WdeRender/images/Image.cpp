#include "Image.hpp"
#include "../../WaterDropEngine.hpp"
#include "../buffers/BufferUtils.hpp"

namespace wde::render {
	Image::Image(VkImageType type, VkImageViewType viewType, uint32_t arrayLayers, VkFormat format, VkExtent3D extent, uint32_t mipLevels, VkSampleCountFlagBits samplesCount, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkImageAspectFlags imageAspect, uint32_t baseMipLevel, uint32_t baseArrayLayer, uint32_t layerCount, bool initialize)
			: _type(type), _viewType(viewType), _arrayLayers(arrayLayers), _format(format), _extent(extent), _mipLevels(mipLevels), _samplesCount(samplesCount), _tiling(tiling), _usage(usage), _memoryProperties(memoryProperties), _imageAspect(imageAspect), _baseMipLevel(baseMipLevel), _baseArrayLayer(baseArrayLayer), _layerCount(layerCount) {
		if (format == VK_FORMAT_UNDEFINED)
			throw WdeException(LogChannel::RENDER, "The specified image format is undefined.");

		if (initialize) {
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating a Vulkan image." << logger::endl;
			createImage();

			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating a Vulkan image view." << logger::endl;
			createImageView();
		}
	}

	Image::~Image() {
		WDE_PROFILE_FUNCTION();
		VkDevice &device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

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
		imageCreateInfo.samples = _samplesCount;
		imageCreateInfo.tiling = _tiling;
		imageCreateInfo.usage = _usage;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		_layout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkDevice &device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();
		if (vkCreateImage(device, &imageCreateInfo, nullptr, &_image) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create image.");

		// Create image memory
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(device, _image, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = BufferUtils::findMemoryType(
				WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(), memoryRequirements.memoryTypeBits, _memoryProperties);
		if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &_memory) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to allocate image memory.");

		// Binding image memory to image
		if (vkBindImageMemory(device, _image, _memory, 0) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to bind image memory to image.");
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

		if (vkCreateImageView(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &viewInfo, nullptr, &_view) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create texture image view.");
	}



	VkFormat Image::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (const auto &format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		// Format not found in candidates
		logger::log(LogLevel::WARN, LogChannel::RENDER) << "Didn't found required image format." << logger::endl;
		return VK_FORMAT_UNDEFINED;
	}

	void Image::insertImageMemoryBarrier(CommandBuffer& commandBuffer, VkImage &image, VkAccessFlags srcAccessMasks,
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