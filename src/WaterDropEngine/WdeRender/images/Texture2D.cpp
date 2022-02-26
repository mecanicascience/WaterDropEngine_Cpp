#include "Texture2D.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../lib/stb/stb_image.h"
#include "../../WaterDropEngine.hpp"

namespace wde::render {

	Texture2D::Texture2D(glm::vec2 imageExtent, VkFormat textureFormat, VkImageUsageFlags textureUsage, VkFilter textureFilter, VkSamplerAddressMode textureAddressMode)
		: _filepath(), _textureFormat(textureFormat), _imageExtent(VkExtent2D {static_cast<uint32_t>(imageExtent.x), static_cast<uint32_t>(imageExtent.y)}), _textureUsage(textureUsage) {
		// Create the texture image
		createTextureImage();

		// Create the texture layout
		_textureImage->createImageView();

		// Create the texture sampler
		createTextureSampler(textureFilter, textureAddressMode);
	}

	Texture2D::Texture2D(std::string filepath, VkFormat textureFormat, VkImageUsageFlags textureUsage, VkFilter textureFilter, VkSamplerAddressMode textureAddressMode)
		: _filepath(std::move(filepath)), _textureFormat(textureFormat), _imageExtent(VkExtent2D {0, 0}), _textureUsage(textureUsage) {
		// Create the texture image
		createTextureImage();

		// Create the texture layout
		_textureImage->createImageView();

		// Create the texture sampler
		createTextureSampler(textureFilter, textureAddressMode);
	}

	Texture2D::~Texture2D() {
		// Destroy texture sampler
		vkDestroySampler(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _textureSampler, nullptr);
	};


	// Core functions
	void Texture2D::createTextureImage() {
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice();

		// Load the texture
		if (!_filepath.empty()) {
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Loading texture from path '" + _filepath + "'." << logger::endl;
			int texChannels;
			int texWidth;
			int texHeight;

			stbi_uc* pixels = stbi_load(_filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			VkDeviceSize imageSize = texWidth * texHeight * 4;

			if (!pixels)
				throw WdeException(LogChannel::RENDER, "Failed to load texture image.");

			// Create staging buffer
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			BufferUtils::createBuffer(device.getPhysicalDevice(), device.getDevice(), imageSize,
			                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			                          stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(device.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(imageSize));
			vkUnmapMemory(device.getDevice(), stagingBufferMemory);

			stbi_image_free(pixels); // clean-up pixel array

			// Create image
			VkExtent2D extent = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)};
			_textureImage = std::make_unique<Image2D>(_textureFormat, extent, VK_IMAGE_USAGE_TRANSFER_DST_BIT | _textureUsage, false);
			_textureImage->createImage();

			// Transition layouts and copy image buffer to texture image
			transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			BufferUtils::copyBufferToImage(stagingBuffer, _textureImage->getImage(), static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

			if ((_textureUsage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT) // Transition to image storage usage
				transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
				// Default (transition to image sampled and other)
			else
				transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


			// Free staging buffers
			vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
			vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
		}

			// Create a new empty texture
		else {
			logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating an empty texture." << logger::endl;

			// Create image
			_textureImage = std::make_unique<Image2D>(_textureFormat, _imageExtent, _textureUsage, false);
			_textureImage->createImage();

			// Transition image to used layout
			if ((_textureUsage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT) // Transition to image storage usage
				transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			else // Default (transition to image sampled and other)
				transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	void Texture2D::createTextureSampler(VkFilter sampler, VkSamplerAddressMode addressMode) {
		// Create samples
		VkSamplerCreateInfo samplerInfo {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = sampler;
		samplerInfo.minFilter = sampler;

		// Adress sample mode
		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;

		// Anisotropic filtering (avoid blurring textures)
		VkPhysicalDeviceProperties properties {};
		vkGetPhysicalDeviceProperties(WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(), &properties);
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		// Border texture color
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		// Use UV coordinates (0-1)
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		// Compare function to apply to texel - percentage close filtering (currently disabled)
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		// Mipmapping
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		// Create sampler
		if (vkCreateSampler(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &samplerInfo, nullptr, &_textureSampler) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create texture sampler.");
	}




	// Helper
	void Texture2D::toLayout(VkImageLayout layout) {
		if (layout == _textureImage->getLayout())
			return;

		transitionImageLayout(*_textureImage, _textureImage->getLayout(), layout);
	}

	void Texture2D::transitionImageLayout(Image &image, VkImageLayout oldLayout, VkImageLayout newLayout) {
		// Create a temporary command buffer
		CommandBuffer commandBuffer {false, VK_COMMAND_BUFFER_LEVEL_PRIMARY};
		commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		// Create memory barrier (synchronize accesses to² resources)
		VkImageMemoryBarrier barrier {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Not use transfer queue
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Not use transfer queue
		barrier.image = image.getImage();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		// Not an array and doesn't have mip levels
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;


		// Set barrier source and destination masks sieges
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;


		// From undefined
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) { // && newLayout == VK_IMAGE_LAYOUT_GENERAL
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}

			// From transfer distance
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

			// From general layout
		else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) { // && oldLayout == VK_IMAGE_LAYOUT_GENERAL
			barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

			// Not implemented
		else
			throw WdeException(LogChannel::RENDER, "Layout transition currently not implemented.");

		// Push barrier to the command buffer
		vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier // Use only memory barrier
		);

		// Submit command buffer
		commandBuffer.end();
		commandBuffer.submit();
		commandBuffer.waitForQueueIdle();

		// Set new layout info
		image.setLayout(newLayout);
	}
}
