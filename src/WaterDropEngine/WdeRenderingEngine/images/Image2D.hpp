#pragma once

#include <vulkan/vulkan_core.h>
#include "Image.hpp"

namespace wde::renderEngine {
	class Image2D : public Image, NonCopyable {
		public:
			explicit Image2D(VkFormat format, VkExtent2D imageExtent)
			: Image(VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, 1, format,
					{imageExtent.width, imageExtent.height, 1}, 1, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1) {
				WDE_PROFILE_FUNCTION();
			}
	};
}
