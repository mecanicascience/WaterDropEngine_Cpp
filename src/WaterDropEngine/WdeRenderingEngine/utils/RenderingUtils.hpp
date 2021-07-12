#pragma once

#include <vulkan/vulkan_core.h>

#include "../../WdeCommon/WdeError/WdeException.hpp"

namespace wde::renderEngine {
	class RenderingUtils {
		public:
			static void createBuffer(VkPhysicalDevice &physicalDevice, VkDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);

			static uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static void copyBuffer(VkBuffer pT, VkBuffer pT1, VkDeviceSize size);
	};
}
