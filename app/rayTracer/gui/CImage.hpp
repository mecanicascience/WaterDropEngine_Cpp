#pragma once

#include "../../src/wde.hpp"

namespace wde::render {
	enum class ImageFormat {
		None = 0,
		RGBA,
		RGBA32F
	};


	class CImage {
		public:
			explicit CImage(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
			~CImage();

			// Getters and setters
			VkDescriptorSet getDescriptorSet() const { return _descriptorSet; }
			uint32_t getWidth() const { return _width; }
			uint32_t getHeight() const { return _height; }

			// Update image pixels
			void setData(const void* data);


		private:
			uint32_t _width = 0;
			uint32_t _height = 0;
			VkImage _image = nullptr;
			VkImageView _imageView = nullptr;
			VkDeviceMemory _memory = nullptr;
			VkSampler _sampler = nullptr;
			ImageFormat _format = ImageFormat::None;
			VkBuffer _stagingBuffer = nullptr;
			VkDeviceMemory _stagingBufferMemory = nullptr;
			size_t _alignedSize = 0;
			VkDescriptorSet _descriptorSet = nullptr;
			std::string _filepath;

			// Custom image creation
			void allocateMemory(uint64_t size);
	};
}
