#pragma once

namespace wde::renderEngine {
	class BufferUtils {
		public:
			/**
			 * @param physicalDevice
			 * @param device
			 * @param size
			 * @param usage
			 * @param properties
			 * @param buffer
			 * @param bufferMemory
			 */
			static void createBuffer(VkPhysicalDevice &physicalDevice, VkDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
				// Create buffer infos
				VkBufferCreateInfo  bufferInfo {};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = size;
				bufferInfo.usage = usage;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

				// Create buffer
				if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
					throw WdeException("Failed to create buffer.", LoggerChannel::RENDERING_ENGINE);

				// Allocate memory
				VkMemoryRequirements memRequirements;
				vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

				VkMemoryAllocateInfo  allocInfo {};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

				if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
					throw WdeException("Failed to allocate buffer memory.", LoggerChannel::RENDERING_ENGINE);

				vkBindBufferMemory(device, buffer, bufferMemory, 0);
			}


			/**
			 * Finds the memory type of the given physical device
			 * @param physicalDevice
			 * @param typeFilter The memory requirements
			 * @param properties The memory requested properties
			 * @return the memory ID
			 */
			static uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
				for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
					if ((typeFilter & (1 << i)) &&
					    (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
						return i;
					}
				}

				throw WdeException("Failed to find suitable memory type.", LoggerChannel::RENDERING_ENGINE);
			}
	};
}
