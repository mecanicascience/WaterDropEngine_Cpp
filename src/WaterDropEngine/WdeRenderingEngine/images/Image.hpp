#pragma once

#include "../../../wde.hpp"

namespace wde::renderEngine {
    /**
     * Class that represents a Vulkan image
     */
    class Image {
        public:
            // Constructor
            /**
             * Create a new image
             * @param imageExtent The corresponding image extent
             * @param imageFormat
             */
            explicit Image(VkExtent2D imageExtent, VkFormat imageFormat);
            ~Image();

            // Getters and setters
            VkFormat getFormat() { return _format; }
            VkImageView getView() { return _view; }


        protected:
            /**
             * Choose the first supported format
             * @param candidates
             * @param tiling
             * @param features
             * @return The image format
             */
            static VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);


        private:
            VkFormat _format = VK_FORMAT_UNDEFINED;
            VkImageView _view = VK_NULL_HANDLE;
            VkExtent2D _imageExtent;


            // Core functions
            void createImage();
            void createImageView();
    };
}
