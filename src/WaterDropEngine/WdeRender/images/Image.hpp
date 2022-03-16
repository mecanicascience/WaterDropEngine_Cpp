#pragma once

#include "../../../wde.hpp"
#include "../commands/CommandBuffer.hpp"

namespace wde::render {
	/**
	 * Class that represents a Vulkan image
	 */
	class Image : public NonCopyable {
		public:
			// Constructor
			/**
			 * Create a new image
			 * @param extent Size of the image
			 * @param type Type of the image (2D, 3D, ...)
			 * @param viewType View type of the image (2D, 3D, ...)
			 * @param imageAspect Aspect of the image (color image, depth image, ...)
			 * @param usage The usage of the image (stencil image, color attachment, ...)
			 * @param format Storage format of the image (rgb, rgba, ...)
			 * @param tiling Can be VK_IMAGE_TILING_LINEAR or VK_IMAGE_TILING_OPTIMAL
			 * @param samplesCount Number of image samples (MSAA Sampling)
			 * @param mipLevels Number of mipmaps image levels
			 * @param arrayLayers List of array layers
			 * @param layerCount Number of array layers
			 * @param initialize Should initialize the image directly on it's creation (default true)
			 */
			explicit Image(VkExtent3D extent, VkImageType type, VkImageViewType viewType, VkImageAspectFlags imageAspect,
			               VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, VkSampleCountFlagBits samplesCount,
			               uint32_t mipLevels, uint32_t arrayLayers, uint32_t layerCount, bool initialize = true);
			~Image() override;


			// Core functions
			void createImage();
			void createImageView();

			// Getters and setters
			VkFormat& getFormat() { return _format; }
			VkImageView& getView() { return _view; }
			VkImage& getImage() { return _image; }
			VkDeviceMemory& getMemory() { return _memory; }
			void setLayout(VkImageLayout layout) { _layout = layout; }
			VkImageLayout& getLayout() { return _layout; }
			uint32_t getMipLevelsCount() const { return _mipLevels; }


			// Helper functions
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
			VkImageLayout _layout = VK_IMAGE_LAYOUT_UNDEFINED;

			// Image description values
			VkImageType _type;
			VkImageViewType _viewType;
			uint32_t _arrayLayers;
			VkFormat _format;
			VkExtent3D _extent;
			uint32_t _mipLevels;
			VkSampleCountFlagBits _samplesCount;
			VkImageTiling _tiling;
			VkImageUsageFlags _usage;
			VkImageAspectFlags _imageAspect;
			uint32_t _layerCount;
	};
}
