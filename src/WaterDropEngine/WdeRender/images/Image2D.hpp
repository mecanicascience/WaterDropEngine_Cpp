#pragma once

#include "../../../wde.hpp"
#include "Image.hpp"

namespace wde::render {
	/**
	 * Represents a 2D image
	 */
	class Image2D : public Image {
		public:
			/**
			 * Create a new 2D image
			 * @param format
			 * @param imageExtent
			 * @param usage (default : Color attachment and storage)
			 * @param initialize Should create extent and frame (default true)
			 */
			explicit Image2D(VkFormat format, VkExtent2D imageExtent, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, bool initialize = true)
					: Image(VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, 1, format,
					        {imageExtent.width, imageExtent.height, 1}, 1, VK_IMAGE_TILING_OPTIMAL,
					        usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
					        0, 0, 1, initialize) {}
	};
}

