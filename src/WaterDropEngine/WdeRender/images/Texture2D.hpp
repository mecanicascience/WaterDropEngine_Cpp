#pragma once

#include "../../../wde.hpp"
#include "Image2D.hpp"
#include "../buffers/BufferUtils.hpp"

namespace wde::render {
	/**
	 * Describes and stores a 2D Texture
	 */
	class Texture2D : public NonCopyable {
		public:
			// Constructors
			/**
			 * Creates a new empty texture
			 * @param imageExtent The size of the texture image (width / height)
			 * @param textureFormat The format of the texture (default VK_FORMAT_R8G8B8A8_SRGB)
			 * @param textureUsage The usage of the texture (default as sampled image)
			 * @param textureFilter The optional image filter (default linear filter)
			 * @param textureAddressMode The repeat image mode (default repeat image)
			 */
			explicit Texture2D(glm::vec2 imageExtent, VkFormat textureFormat = VK_FORMAT_R8G8B8A8_SRGB, VkImageUsageFlags textureUsage = VK_IMAGE_USAGE_SAMPLED_BIT,
			                   VkFilter textureFilter = VK_FILTER_LINEAR, VkSamplerAddressMode textureAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);


			/**
			 * Creates a new texture given a file path
			 * @param filepath Location path of the texture
			 * @param textureFormat The format of the texture (default VK_FORMAT_R8G8B8A8_SRGB)
			 * @param textureUsage The usage of the texture (default as sampled image)
			 * @param textureFilter The optional image filter (default linear filter)
			 * @param textureAddressMode The repeat image mode (default repeat image)
			 */
			explicit Texture2D(std::string filepath, VkFormat textureFormat = VK_FORMAT_R8G8B8A8_SRGB, VkImageUsageFlags textureUsage = VK_IMAGE_USAGE_SAMPLED_BIT,
			                   VkFilter textureFilter = VK_FILTER_LINEAR, VkSamplerAddressMode textureAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

			~Texture2D() override;


			// Helper functions
			/**
			 * Transition between from one layout to another
			 * @param image The image that will transition formats
			 * @param oldLayout Old layout of the image
			 * @param newLayout New layout of the image
			 */
			static void transitionImageLayout(Image &image, VkImageLayout oldLayout, VkImageLayout newLayout);
			/**
			 * Loads an image given a link
			 * @param image Link to the image
			 * @param width Width of the image that will be set
			 * @param height Height of the image that will be set
			 * @param channelsCount Number of channels in the image
			 * @return The image pixels
			 */
			static unsigned char *getImagePixels(const std::string& image, int& width, int& height, int channelsCount);
			/**
			 * Free the pixels of a given imageq
			 * @param pixels
			 */
			static void freeImagePixels(unsigned char* pixels);


			// Getters and setters
			VkExtent2D getExtent() const { return _imageExtent; }
			VkDescriptorImageInfo createDescriptor(VkImageLayout layout) const {
				VkDescriptorImageInfo imageInfo {};
				imageInfo.imageLayout = layout;
				imageInfo.imageView = _textureImage->getView();
				imageInfo.sampler = _textureSampler;
				return imageInfo;
			}



		private:
			// Texture parameters
			std::string _filepath;
			VkFormat _textureFormat;
			VkExtent2D _imageExtent;
			VkImageUsageFlags _textureUsage;
			uint32_t _mipLevels = 1;

			// Texture data
			std::unique_ptr<Image2D> _textureImage;
			VkSampler _textureSampler {};


			// Core functions
			/** Create the texture image */
			void createTextureImage();
			/**
			 * Create the optional image texture samples
			 * @param sampler Tiling sampler (VK_FILTER_NEAREST for oversampling, VK_FILTER_LINEAR for undersampling, VK_FILTER_LINEAR for none (default))
			 * @param addressMode The repeating pattern of the image (VK_SAMPLER_ADDRESS_MODE_REPEAT for repeat (default), VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT to repeat as mirror,
			 * VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE take the nearest color, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER return a solid color outside)
			 */
			void createTextureSampler(VkFilter sampler = VK_FILTER_LINEAR, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
	};
}
