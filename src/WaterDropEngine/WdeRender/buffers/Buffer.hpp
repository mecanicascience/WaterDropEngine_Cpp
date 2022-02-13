#pragma once

#include "../../../wde.hpp"

namespace wde::render {
	/**
	 * Class that stores a buffer and it's different components
	 */
	class Buffer {
		public:
			// Constructors
			/**
			 * Create a new buffer
			 * @param size Size of the buffer
			 * @param usage The buffer usage flags
			 * @param properties The buffer memory property flags (default : host visible, and host coherent)
			 */
			explicit Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
					: _bufferSize(size), _bufferUsageFlags(usage), _memoryProperties(properties) {
				WDE_PROFILE_FUNCTION();
				// Create the buffer
				createBuffer();
			}

			/** Clean up the buffer */
			~Buffer();


			// Core functions
			/** @return the mapped buffer */
			void* map();

			/** Unmap the buffer memory */
			void unmap();


			// Getters and setters
			VkBuffer& getBuffer() { return _buffer; }
			VkDeviceMemory& getMemory() { return _bufferMemory; }
			uint32_t getSize() { return _bufferSize; }



		private:
			// Buffer configuration
			VkDeviceSize _bufferSize;
			VkBufferUsageFlags _bufferUsageFlags;
			VkMemoryPropertyFlags _memoryProperties;

			// Buffer components
			VkBuffer _buffer = VK_NULL_HANDLE;
			VkDeviceMemory _bufferMemory = VK_NULL_HANDLE;


			// Core functions
			/** Create the buffer and it's memory */
			void createBuffer();
	};
}
