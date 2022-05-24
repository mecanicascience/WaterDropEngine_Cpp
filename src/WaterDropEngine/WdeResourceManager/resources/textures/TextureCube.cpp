#include "TextureCube.hpp"
#include "../../../../../lib/stb/stb_image.h"
#include "../../../WaterDropEngine.hpp"

namespace wde::resource {
	TextureCube::TextureCube(const std::string &path) : Resource(path, ResourceType::IMAGE) {
		WDE_PROFILE_FUNCTION();
		auto texData = json::parse(WdeFileUtils::readFile(path));
		if (texData["type"] != "image" || texData["data"]["type"] != "cube")
			throw WdeException(LogChannel::RES, "Trying to create a cube-texture from a non-cube-texture description.");
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice();

		// Load first image to fetch sizes
		int texWidth;
		int texHeight;
		{
			int texChannels;
			auto pathResSrc = WaterDropEngine::get().getInstance().getScene()->getPath() + "data/textures/"
			            + texData["data"]["path"].get<std::string>()
			            + "/front."
						+ texData["data"]["extension"].get<std::string>();
			stbi_uc* pixels = stbi_load(pathResSrc.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			if (!pixels)
				throw WdeException(LogChannel::RENDER, "Failed to load texture image.");
			stbi_image_free(pixels); // clean-up pixel array
		}

		// Set image data
		const VkDeviceSize imageSize = texWidth * texHeight * 4 * 6;
		const VkDeviceSize layerSize = imageSize / 6;
		uint32_t mipLevels = 1;

		// Create image
		{
			WDE_PROFILE_SCOPE("wde::resource::TextureCube::TextureCube::createImage()");
			// Create image
			VkExtent3D extent = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1};
			_textureImage = std::make_unique<render::Image>(extent, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_CUBE,
			                                        VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			                                        VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_SAMPLE_COUNT_1_BIT, mipLevels, 6, 6, false);
			_textureImage->createImage();

			// Transition layouts and copy image buffer to texture image
			transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		}

		// Create staging buffer and copy texture data to it
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		std::vector<std::string> textureName {"right", "left", "top", "bottom", "front", "back"};
		{
			WDE_PROFILE_SCOPE("wde::resource::TextureCube::TextureCube::createBuffers()");
			render::BufferUtils::createBuffer(device.getPhysicalDevice(), device.getDevice(), layerSize,
			                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			                          stagingBuffer, stagingBufferMemory);

			// Map
			void* data;
			vkMapMemory(device.getDevice(), stagingBufferMemory, 0, layerSize, 0, &data);

			for (int face = 0; face < 6; face++) {
				WDE_PROFILE_SCOPE("wde::resource::TextureCube::TextureCube::loadFace()");
				// Load texture and copy data to buffer
				int texChannels;
				stbi_uc* pixels = stbi_load((WaterDropEngine::get().getInstance().getScene()->getPath() + "data/textures/"
				                             + texData["data"]["path"].get<std::string>()
				                             + "/" + textureName[face] + "."
											 + texData["data"]["extension"].get<std::string>()).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
				if (!pixels)
					throw WdeException(LogChannel::RENDER, "Failed to load texture image.");
				memcpy(data, pixels, static_cast<size_t>(layerSize));
				stbi_image_free(pixels);

				// Copy data
				render::BufferUtils::copyBufferToImage(stagingBuffer, _textureImage->getImage(),
				                               static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), face);
			}

			// Unmap
			vkUnmapMemory(device.getDevice(), stagingBufferMemory);


			// Free staging buffers
			vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
			vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
		}

		// Create the texture layout
		_textureImage->createImageView();

		// Create texture sampler
		{
			WDE_PROFILE_SCOPE("wde::resource::TextureCube::TextureCube::createSamplers()");
			// Create samples
			VkSamplerCreateInfo samplerInfo {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;

			// Address sample mode
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

			// Anisotropic filtering (avoid blurring textures)
			VkPhysicalDeviceProperties properties {};
			vkGetPhysicalDeviceProperties(WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(), &properties);
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

			// Border texture color
			samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

			// Use UV coordinates (0-1)
			samplerInfo.unnormalizedCoordinates = VK_FALSE;

			// Compare function to apply to texel - percentage close filtering (currently disabled)
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

			// Mip mapping
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // Suppose linear mode active on device
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = static_cast<float>(mipLevels);

			// Create sampler
			if (vkCreateSampler(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &samplerInfo, nullptr, &_textureSampler) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create texture sampler.");
		}

		// Transition to displaying layout
		transitionImageLayout(*_textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


		// Generate 2D images for GUI drawing (these images should NOT CHANGE or changes will not be seen)
#ifdef WDE_GUI_ENABLED
		{
			WDE_PROFILE_SCOPE("wde::resource::TextureCube::TextureCube::createGUIImages()");
			// Generate GUI Debug Images
			for (int face = 0; face < 6; face++) {
				const std::string pathGUI = WaterDropEngine::get().getInstance().getScene()->getPath() + "data/textures/" + texData["data"]["path"].get<std::string>()
				                         + "/" + textureName[face] + "." + texData["data"]["extension"].get<std::string>();
				_textureImageGUI.push_back(std::make_unique<Texture2D>(pathGUI, true));
			}
		}
#endif
	}

	TextureCube::~TextureCube() {
		WDE_PROFILE_FUNCTION();
		// Destroy texture sampler
		vkDestroySampler(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _textureSampler, nullptr);
	}

	void TextureCube::drawGUI() {
#ifdef WDE_GUI_ENABLED
		WDE_PROFILE_FUNCTION();

		// Faces
		for (int i = 0; i < 6; i++) {
			ImGui::Image(_textureImageGUI[i]->getGUIID(), ImVec2(200.0f, 200.0f));
			if (i != 5) {
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(5.0f, 0.0f));
				ImGui::SameLine();
			}
		}

		// Image data
		ImGui::Dummy(ImVec2(8.0f, 0.0f));
		ImGui::Text("Image data:");
		ImGui::Text("  - Small texture sizes : %u x %u", _textureImageGUI[0]->getExtent().width, _textureImageGUI[0]->getExtent().height);
		ImGui::Text("  - Format : %i", _textureImage->getFormat());
		ImGui::Text("  - Reference Count : %i", _referenceCount);
#endif
	}






	// Helper
	void TextureCube::transitionImageLayout(render::Image &image, VkImageLayout oldLayout, VkImageLayout newLayout, int layerCount) {
		WDE_PROFILE_FUNCTION();
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
		barrier.subresourceRange.layerCount = layerCount;


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
