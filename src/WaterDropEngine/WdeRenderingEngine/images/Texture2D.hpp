#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../lib/stb/stb_image.h"
#include <utility>

#include "../../../wde.hpp"
#include "Image2D.hpp"

namespace wde::renderEngine {
	/**
	 * Class that handles a 2D texture
	 */
	class Texture2D {
		public:
			// Constructors
            /**
             * Creates a new empty texture
             * @param imageExtent The size of the texture image
             * @param textureUsage The usage of the texture (default as sampled image)
             * @param textureFormat The format of the texture (default VK_FORMAT_R8G8B8A8_SRGB)
             * @param textureFilter The optional image filter (default linear filter)
             * @param textureAdressMode The repeat image mode (default repeat image)
             */
            explicit Texture2D(VkExtent2D imageExtent, VkFormat textureFormat = VK_FORMAT_R8G8B8A8_SRGB, VkImageUsageFlags textureUsage = VK_IMAGE_USAGE_SAMPLED_BIT,
                               VkFilter textureFilter = VK_FILTER_LINEAR, VkSamplerAddressMode textureAdressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);


            /**
			 * Creates a new texture given a file path
			 * @param filepath Absolute project file path to the texture
             * @param textureUsage The usage of the texture (default as sampled image)
			 * @param textureFormat The format of the texture (default VK_FORMAT_R8G8B8A8_SRGB)
			 * @param textureFilter The optional image filter (default linear filter)
			 * @param textureAdressMode The repeat image mode (default repeat image)
			 */
			explicit Texture2D(std::string filepath, VkFormat textureFormat = VK_FORMAT_R8G8B8A8_SRGB, VkImageUsageFlags textureUsage = VK_IMAGE_USAGE_SAMPLED_BIT,
							   VkFilter textureFilter = VK_FILTER_LINEAR, VkSamplerAddressMode textureAdressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

			/** Texture destructor */
			~Texture2D();

			// Helper functions
			/**
			 * Take a screenshot of the texture and saves it to a file
			 * @param filePath
			 */
			void takeScreenshot(const std::string& filePath) {
			    _textureImage->takeScreenshot(filePath);
			}

			// Getters and setters
			VkImageView& getView() { return _textureImage->getView(); }
			VkSampler& getSampler() { return _textureSampler; }


		private:
			// Texture parameters
			std::string _filepath;
			VkFormat _textureFormat;
			VkExtent2D _imageExtent;
			VkImageUsageFlags _textureUsage;

			// Texture data
			std::unique_ptr<Image2D> _textureImage;
			VkSampler _textureSampler {};


			// Core functions
			/** Create the texture image */
			void createTextureImage();
			/**
			 * Create the optional image texture samples
			 * @param sampler Tiling sampler (VK_FILTER_NEAREST for oversampling, VK_FILTER_LINEAR for undersampling, VK_FILTER_LINEAR for none (default))
			 * @param adressMode The repeating pattern of the image (VK_SAMPLER_ADDRESS_MODE_REPEAT for repeat (default), VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT to repeat as mirror,
			 * VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE take the nearest color, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER return a solid color outside)
			 */
			void createTextureSampler(VkFilter sampler = VK_FILTER_LINEAR, VkSamplerAddressMode adressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);



			// Helper functions
			/**
			 * Transition between from one layout to another
			 * @param image The image that will transition formats
			 * @param oldLayout Old layout of the image
			 * @param newLayout New layout of the image
			 */
			static void transitionImageLayout(Image &image, VkImageLayout oldLayout, VkImageLayout newLayout);
	};
}

