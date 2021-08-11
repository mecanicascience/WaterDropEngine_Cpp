#pragma once

#include <vulkan/vulkan_core.h>
#include "Image.hpp"

namespace wde::renderEngine {
	/**
	 * Represents a 2D image
	 */
	class Image2D : public Image, NonCopyable {
		public:
			explicit Image2D(VkFormat format, VkExtent2D imageExtent, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, bool initialize = true)
				: Image(VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, 1, format,
						{imageExtent.width, imageExtent.height, 1}, 1, VK_IMAGE_TILING_OPTIMAL,
						usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
						0, 0, 1, initialize) {}
	};
}
