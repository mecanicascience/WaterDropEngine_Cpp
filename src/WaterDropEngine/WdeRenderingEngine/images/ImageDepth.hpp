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
            ImageDepth(VkExtent2D imageExtent)
                : Image(imageExtent, findSupportedFormat(DEPTH_TRY_FORMATS, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {}
    };
}
