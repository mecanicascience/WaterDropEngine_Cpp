#pragma once

#include "../../../wde.hpp"
#include "Image.hpp"

namespace wde::renderEngine {
    // The possible depth formats
    static const std::vector<VkFormat> DEPTH_TRY_FORMATS = {
            VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM
    };

    /**
     * An image representing a depth texture
     */
    class ImageDepth : public Image, NonCopyable {
        public:
            /**
             * Create a new image view
             * @param imageExtent
             */
            explicit ImageDepth(VkExtent2D imageExtent)
                : Image(VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, 1, findSupportedFormat(DEPTH_TRY_FORMATS,
							VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
                        {imageExtent.width, imageExtent.height, 1}, 1, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, 1) {
                WDE_PROFILE_FUNCTION();
            }
    };
}
