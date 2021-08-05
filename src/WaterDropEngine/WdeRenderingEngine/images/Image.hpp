#pragma once

#include "../../../wde.hpp"
#include "../buffers/BufferUtils.hpp"

namespace wde::renderEngine {
    /**
     * Class that represents a Vulkan image
     */
    class Image {
        public:
            // Constructor
            /**
             * Create a new image
             * @param type
             * @param viewType
             * @param arrayLayers
             * @param format
             * @param extent
             * @param mipLevels
             * @param tiling
             * @param usage
             * @param memoryProperties
             * @param imageAspect
             * @param baseMipLevel
             * @param baseArrayLayer
             * @param layer
             */
            explicit Image(VkImageType type, VkImageViewType viewType, uint32_t arrayLayers, VkFormat format,
						   VkExtent3D extent, uint32_t mipLevels, VkImageTiling tiling, VkImageUsageFlags usage,
						   VkMemoryPropertyFlags memoryProperties, VkImageAspectFlags imageAspect, uint32_t baseMipLevel,
						   uint32_t baseArrayLayer, uint32_t layerCount);
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
            // Vulkan image values
            VkImage _image = VK_NULL_HANDLE;
            VkImageView _view = VK_NULL_HANDLE;
            VkDeviceMemory _memory = VK_NULL_HANDLE;

            // Image description values
            VkImageType _type;
            VkImageViewType _viewType;
            uint32_t _arrayLayers;
            VkFormat _format;
            VkExtent3D _extent;
            uint32_t _mipLevels;
            VkImageTiling _tiling;
            VkImageUsageFlags _usage;
            VkMemoryPropertyFlags _memoryProperties;
            VkImageAspectFlags _imageAspect;
            uint32_t _baseMipLevel;
            uint32_t _baseArrayLayer;
            uint32_t _layerCount;


            // Core functions
            void createImage();
            void createImageView();
    };
}
