#pragma once

#include "../../Resource.hpp"
#include "../../../WdeRender/images/Image.hpp"
#include "../../../WdeRender/images/Image2D.hpp"
#include "../../../WdeRender/buffers/BufferUtils.hpp"

namespace wde::resource {
	class Texture2D : public Resource {
		public:
			explicit Texture2D(const std::string &path);
			~Texture2D();


			// Helper functions
			/**
			 * Transition between from one layout to another
			 * @param image The image that will transition formats
			 * @param oldLayout Old layout of the image
			 * @param newLayout New layout of the image
			 */
			static void transitionImageLayout(render::Image &image, VkImageLayout oldLayout, VkImageLayout newLayout);
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
			VkFormat _textureFormat {};
			VkExtent2D _imageExtent {};
			VkImageUsageFlags _textureUsage {};
			uint32_t _mipLevels = 1;

			// Texture data
			std::unique_ptr<render::Image2D> _textureImage {};
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
