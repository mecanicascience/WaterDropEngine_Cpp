#include "Texture2D.hpp"
#include "../../../WaterDropEngine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../../lib/stb/stb_image.h"


namespace wde::resource {
	Texture2D::Texture2D(const std::string &path) : Resource(path, ResourceType::IMAGE) {
		WDE_PROFILE_FUNCTION();
		auto texData = json::parse(WdeFileUtils::readFile(path));
		if (texData["type"] != "image" || texData["data"]["type"] != "2D")
			throw WdeException(LogChannel::RES, "Trying to create a 2D-texture from a non-2D-texture description.");

		_filepath = WaterDropEngine::get().getInstance().getScene()->getPath() + "data/textures/" + texData["data"]["path"].get<std::string>();
		_textureFormat = texData["data"]["format"].get<VkFormat>();
		_imageExtent = VkExtent2D {0, 0};
		_textureUsage = texData["data"]["usage_flags"].get<VkImageUsageFlags>();

		// Create the texture image
		createTextureImage();

		// Create the texture layout
		_textureImage->createImageView();

		// Create the texture sampler
		_samplerFilter = texData["data"]["filter"].get<VkFilter>();
		_samplerAddressMode = texData["data"]["adress_mode"].get<VkSamplerAddressMode>();
		createTextureSampler(_textureSampler, _mipLevels, _samplerFilter, _samplerAddressMode);

#ifdef WDE_GUI_ENABLED
		// Generate texture ID
		_textureGUIID = (ImTextureID) ImGui_ImplVulkan_AddTexture(_textureSampler, _textureImage->getView(), _textureImage->getLayout());
#endif
	}
	Texture2D::Texture2D(const std::string &imagePath, bool setDefaultParameters) : Resource(imagePath, ResourceType::IMAGE) {
		WDE_PROFILE_FUNCTION();
		_filepath = imagePath;
		_textureFormat = VK_FORMAT_R8G8B8A8_UNORM;
		_imageExtent = VkExtent2D {0, 0};
		_textureUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		// Create the texture image
		createTextureImage();

		// Create the texture layout
		_textureImage->createImageView();

		// Create the texture sampler
		createTextureSampler(_textureSampler, _mipLevels);

#ifdef WDE_GUI_ENABLED
		// Generate texture ID
		_textureGUIID = (ImTextureID) ImGui_ImplVulkan_AddTexture(_textureSampler, _textureImage->getView(), _textureImage->getLayout());
#endif
	}

	Texture2D::~Texture2D() {
		WDE_PROFILE_FUNCTION();

		// Destroy texture sampler
		vkDestroySampler(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _textureSampler, nullptr);
	};

	void Texture2D::drawGUI() {
#ifdef WDE_GUI_ENABLED
		WDE_PROFILE_FUNCTION();
		ImGui::Image(_textureGUIID, ImVec2(200.0f, 200.0f));

		// Image data
		ImGui::Dummy(ImVec2(8.0f, 0.0f));
		ImGui::Text("Image data:");
		ImGui::Text("  - Size : %u x %u", _imageExtent.width, _imageExtent.height);
		ImGui::Text("  - Format : %i", _textureFormat);
		ImGui::Text("  - Usage flags : %i", _textureUsage);
		ImGui::Text("  - Filter : %i", _textureUsage);
		ImGui::Text("  - Sampler Filter : %i", _samplerFilter);
		ImGui::Text("  - Sampler Address mode : %i", _samplerAddressMode);
		ImGui::Text("  - Reference Count : %i", _referenceCount);
#endif
	}


	// Core functions
	void Texture2D::createTextureImage() {
		WDE_PROFILE_FUNCTION();
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice();

		// Load the texture
		int texChannels;
		int texWidth;
		int texHeight;

		// Load image
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		{
			stbi_uc* pixels = stbi_load(_filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			VkDeviceSize imageSize = texWidth * texHeight * 4;

			// Set image extent
			_imageExtent = VkExtent2D {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)};

			if (!pixels)
				throw WdeException(LogChannel::RENDER, "Failed to load texture image.");

			// Create staging buffer
			render::BufferUtils::createBuffer(device.getPhysicalDevice(), device.getDevice(), imageSize,
			                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			                          stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(device.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(imageSize));
			vkUnmapMemory(device.getDevice(), stagingBufferMemory);

			stbi_image_free(pixels); // clean-up pixel array
		}


		// Create image
		{
			VkExtent2D extent = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)};
			_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
			_textureImage = std::make_unique<render::Image2D>(_textureFormat, extent,
															  VK_IMAGE_USAGE_TRANSFER_DST_BIT | _textureUsage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			                                                  VK_SAMPLE_COUNT_1_BIT, _mipLevels, false);
			_textureImage->createImage();

			// Transition layouts and copy image buffer to texture image
			transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			render::BufferUtils::copyBufferToImage(stagingBuffer, _textureImage->getImage(), static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));


			// Free staging buffers
			vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
			vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
		}


		// Generate image mipmaps
		{
			// Check if image format supports linear blitting
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(), _textureFormat, &formatProperties);
			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
				throw WdeException(LogChannel::RENDER, "Texture image format does not support linear blitting.");
			}

			// Recording commands command buffer
			render::CommandBuffer cmd {true};

			// Create memory barrier
			VkImageMemoryBarrier barrier {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = _textureImage->getImage();
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;


			// Create image mipmaps levels using blit commands
			int32_t mipWidth = texWidth;
			int32_t mipHeight = texHeight;

			for (uint32_t i = 1; i < _mipLevels; i++) {
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				// Wait for last mipmap to be generated
				vkCmdPipelineBarrier(cmd,
				                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				                     0, nullptr,
				                     0, nullptr,
				                     1, &barrier);


				// Blit for current mipmap level
				VkImageBlit blit {};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				vkCmdBlitImage(cmd,
				               _textureImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				               _textureImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				               1, &blit,
				               VK_FILTER_LINEAR);


				// Barrier for current mimap
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(cmd,
				                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				                     0, nullptr,
				                     0, nullptr,
				                     1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			// Transition layout from transfer dst to shader read only optimal
			barrier.subresourceRange.baseMipLevel = _mipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			if ((_textureUsage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT) { // Transition to image storage usage
				barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
				barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			}

			vkCmdPipelineBarrier(cmd,
			                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			                     0, nullptr,
			                     0, nullptr,
			                     1, &barrier);
			_textureImage->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			// Submit command buffer
			cmd.submitIdle();

			// Transition to layout shader read
			transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	void Texture2D::createTextureSampler(VkSampler& sampler, uint32_t mipLevels, VkFilter filter, VkSamplerAddressMode addressMode) {
		WDE_PROFILE_FUNCTION();
		// Create samples
		VkSamplerCreateInfo samplerInfo {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = filter;
		samplerInfo.minFilter = filter;

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
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // Suppose linear mode active on device
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);

		// Create sampler
		if (vkCreateSampler(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
			throw WdeException(LogChannel::RENDER, "Failed to create texture sampler.");
	}


	// Helper
	void Texture2D::transitionImageLayout(VkImageLayout newLayout) {
		WDE_PROFILE_FUNCTION();
		Texture2D::transitionImageLayout(*_textureImage, _textureImage->getLayout(), newLayout);
		_textureImage->setLayout(newLayout);
	}

	void Texture2D::transitionImageLayout(render::Image &image, VkImageLayout oldLayout, VkImageLayout newLayout) {
		WDE_PROFILE_FUNCTION();
		if (oldLayout == newLayout)
			return;

		// Create a temporary command buffer
		render::CommandBuffer commandBuffer {false, VK_COMMAND_BUFFER_LEVEL_PRIMARY};
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
		barrier.subresourceRange.levelCount = image.getMipLevelsCount();
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;


		// Set barrier source and destination masks sieges
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;


		// Set old layout
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		else
			throw WdeException(LogChannel::RENDER, "Initial layout transition currently not implemented.");

		// Set new layout
		if (newLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
			barrier.dstAccessMask = 0;
			destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (newLayout == VK_IMAGE_LAYOUT_GENERAL) {
			barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		else
			throw WdeException(LogChannel::RENDER, "New layout transition currently not implemented.");


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

	unsigned char* Texture2D::getImagePixels(const std::string& image, int& width, int& height, int channelsCount) {
		WDE_PROFILE_FUNCTION();
		return stbi_load(image.c_str(), &width, &height, nullptr, channelsCount);
	}

	void Texture2D::freeImagePixels(unsigned char* pixels) {
		WDE_PROFILE_FUNCTION();
		stbi_image_free(pixels);
	}
}
