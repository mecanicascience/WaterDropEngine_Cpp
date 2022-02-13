#include "Buffer.hpp"
#include "BufferUtils.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	Buffer::~Buffer() {
		WDE_PROFILE_FUNCTION();
		// Clean up the buffer memory
		vkFreeMemory(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _bufferMemory, nullptr);

		// Clean up the buffer
		vkDestroyBuffer(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _buffer, nullptr);
	}

	void Buffer::createBuffer() {
		// Create the buffer
		BufferUtils::createBuffer(WaterDropEngine::get().getRender().getInstance().getDevice().getPhysicalDevice(),
		                          WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(),
		                          _bufferSize, _bufferUsageFlags, _memoryProperties,
		                          _buffer, _bufferMemory);
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
}