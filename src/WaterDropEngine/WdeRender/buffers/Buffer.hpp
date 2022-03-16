#pragma once

#include "../../../wde.hpp"

namespace wde::render {
	/**
	 * Class that stores a buffer and it's different components
	 */
	class Buffer : public NonCopyable {
		public:
			// Constructors
			/**
			 * Create a new buffer
			 * @param size Size of the buffer
			 * @param usage The buffer usage flags
			 * @param properties The buffer memory property flags (default : host visible, and host coherent)
			 */
			explicit Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			/** Clean up the buffer */
			~Buffer() override;


			// Core functions
			/**
			 * Copy this buffer to another buffer
			 * @param destination
			 */
			void copyTo(Buffer& destination);

			/** @return the mapped buffer */
			void* map();
			/** Unmap the buffer memory */
			void unmap();


			// Getters and setters
			VkBuffer& getBuffer() { return _buffer; }
			VkDeviceMemory& getMemory() { return _bufferMemory; }
			uint32_t getSize() const { return _bufferSize; }
			const VkDescriptorBufferInfo* getBufferInfo() const { return &_bufferInfo; }



		private:
			// Buffer configuration
			VkDeviceSize _bufferSize;
			VkBufferUsageFlags _bufferUsageFlags;
			VkMemoryPropertyFlags _memoryProperties;

			// Buffer components
			VkBuffer _buffer = VK_NULL_HANDLE;
			VkDeviceMemory _bufferMemory = VK_NULL_HANDLE;
			VkDescriptorBufferInfo _bufferInfo {};
	};
}
