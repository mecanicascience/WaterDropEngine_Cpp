#include "Image.hpp"
#include "../../WaterDropEngine.hpp"
#include "../buffers/BufferUtils.hpp"

namespace wde::render {
	Image::Image(VkExtent3D extent, VkImageType type, VkImageViewType viewType, VkImageAspectFlags imageAspect,
	             VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, VkSampleCountFlagBits samplesCount,
	             uint32_t mipLevels, uint32_t arrayLayers, uint32_t layerCount, bool initialize)
			: _type(type), _viewType(viewType), _arrayLayers(arrayLayers), _format(format), _extent(extent), _mipLevels(mipLevels),
			  _samplesCount(samplesCount), _tiling(tiling), _usage(usage), _imageAspect(imageAspect), _layerCount(layerCount) {
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
				WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(), memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = _mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
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
}