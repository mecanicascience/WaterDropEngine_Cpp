#pragma once

#include "../../../wde.hpp"
#include "Image.hpp"
#include "Image2D.hpp"

namespace wde::render {
	/**
	 * Represents a Cube Texture
	 */
	class TextureCube {
		public:
			/**
			 * Creates a new Cube texture
			 * @param textureFile File folder of the 6 cube textures (must end with a '/' char)
			 * @param textureExtension Extension of the images (jpg, png, ...)
			 */
			explicit TextureCube(const std::string& textureFolder, const std::string& textureExtension);
			~TextureCube();


			// Getters and setters
			VkDescriptorImageInfo createDescriptor(VkImageLayout layout) const {
				VkDescriptorImageInfo imageInfo {};
				imageInfo.imageLayout = layout;
				imageInfo.imageView = _textureImage->getView();
				imageInfo.sampler = _textureSampler;
				return imageInfo;
			}

			// Helper functions
			/**
			 * Transition between from one layout to another
			 * @param image The image that will transition formats
			 * @param oldLayout Old layout of the image
			 * @param newLayout New layout of the image
			 */
			static void transitionImageLayout(Image &image, VkImageLayout oldLayout, VkImageLayout newLayout);


		private:
			// Core parameters
			std::unique_ptr<Image> _textureImage;
			VkSampler _textureSampler {};
	};
}
