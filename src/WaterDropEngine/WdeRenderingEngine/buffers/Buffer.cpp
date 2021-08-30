#include "Buffer.hpp"
#include "../WdeRenderEngine.hpp"

namespace wde::renderEngine {
	Buffer::~Buffer() {
		WDE_PROFILE_FUNCTION();
		// Clean up the buffer memory
		vkFreeMemory(WdeRenderEngine::get().getSelectedDevice().getDevice(), _bufferMemory, nullptr);

		// Clean up the buffer
		vkDestroyBuffer(WdeRenderEngine::get().getSelectedDevice().getDevice(), _buffer, nullptr);
	}

	void Buffer::createBuffer() {
		// Create the buffer
		BufferUtils::createBuffer(WdeRenderEngine::get().getSelectedDevice().getPhysicalDevice(),
								  WdeRenderEngine::get().getSelectedDevice().getDevice(),
								  _bufferSize, _bufferUsageFlags, _memoryProperties,
								  _buffer, _bufferMemory);
	}
}
