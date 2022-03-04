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
			 * @param format Storage format of the image (rgb, rgba, ...)
			 * @param imageExtent Size of the image
			 * @param usage Image usage (default : Color attachment and storage)
			 * @param samplesCount Number of samples of MSAA filtering in image (default : 1 bit)
			 * @param mipLevels Number of mipmap levels in the image
			 * @param initialize Should create extent and frame (default true)
			 */
			explicit Image2D(VkFormat format, VkExtent2D imageExtent, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
			                 VkSampleCountFlagBits samplesCount = VK_SAMPLE_COUNT_1_BIT, uint32_t mipLevels = 1, bool initialize = true)
					: Image({imageExtent.width, imageExtent.height, 1}, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT,
					        usage, format, VK_IMAGE_TILING_OPTIMAL, samplesCount, mipLevels, 1, 1, initialize) {}
	};
}

