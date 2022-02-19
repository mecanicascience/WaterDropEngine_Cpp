#include "Buffer.hpp"
#include "BufferUtils.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : _bufferSize(size), _bufferUsageFlags(usage), _memoryProperties(properties) {
		WDE_PROFILE_FUNCTION();
		// Create the buffer
		BufferUtils::createBuffer(WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(),
		                          WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(),
		                          _bufferSize, _bufferUsageFlags, _memoryProperties,
		                          _buffer, _bufferMemory);

		// Create buffer infos
		_bufferInfo.offset = 0;
		_bufferInfo.buffer = _buffer;
		_bufferInfo.range = _bufferSize;
	}

	Buffer::~Buffer() {
		WDE_PROFILE_FUNCTION();
		// Clean up the buffer memory
		vkFreeMemory(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _bufferMemory, nullptr);

		// Clean up the buffer
		vkDestroyBuffer(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _buffer, nullptr);
	}



	// Core functions
	void* Buffer::map() {
		void* bufferData;
		vkMapMemory(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _bufferMemory, 0, _bufferSize, 0, &bufferData);
		return bufferData;
	}

	void Buffer::unmap() {
		vkUnmapMemory(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _bufferMemory);
	}

	void Buffer::copyTo(Buffer &destination) {
		BufferUtils::copyBuffer(WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(),
								_buffer, destination.getBuffer(), _bufferSize);
	}
}