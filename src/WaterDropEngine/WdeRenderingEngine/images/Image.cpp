#include "Image.hpp"

namespace wde::renderEngine {
    Image::Image(VkExtent2D imageExtent, VkFormat imageFormat) : _imageExtent(imageExtent), _format(imageFormat) {
        Logger::debug("Creating a Vulkan image.", LoggerChannel::RENDERING_ENGINE);
        if (imageFormat == VK_FORMAT_UNDEFINED)
            throw WdeException("The specified image format is undefined.", LoggerChannel::RENDERING_ENGINE);

        createImage();
        createImageView();
    }

    Image::~Image() {

    }




    void Image::createImage() {
        // https://github.com/EQMG/Acid/blob/master/Sources/Graphics/Images/Image.cpp
    }

    void Image::createImageView() {

    }



    VkFormat Image::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        return candidates[0];
    }
}
