#pragma once

#include "../../../../wde.hpp"
#include "../../../WdeRender/images/Image.hpp"
#include "../../../WdeRender/images/Image2D.hpp"
#include "../../Resource.hpp"

namespace wde::resource {
	class TextureCube : public Resource {
		public:
			explicit TextureCube(const std::string &path);
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
			static void transitionImageLayout(render::Image &image, VkImageLayout oldLayout, VkImageLayout newLayout);


		private:
			// Core parameters
			std::unique_ptr<render::Image> _textureImage;
			VkSampler _textureSampler {};
	};
}
