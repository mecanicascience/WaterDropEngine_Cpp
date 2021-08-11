#pragma once

#include "../commands/CommandBuffer.hpp"

namespace wde::renderEngine {
	class BufferUtils {
		public:
			/**
			 * @param physicalDevice A reference to the buffer physical device
			 * @param device A reference to the buffer device
			 * @param size Size of the buffer
			 * @param usage Describes how the buffer will be used
			 * @param properties The properties of the buffer
			 * @param buffer A reference to the buffer
			 * @param bufferMemory A reference to the buffer memory
			 */
			static void createBuffer(VkPhysicalDevice &physicalDevice, VkDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
                WDE_PROFILE_FUNCTION();
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
			 * Copy the content of a buffer to another
			 * @param physicalDevice A reference to the buffer physical device
			 * @param srcBuffer The buffer to copy from
			 * @param dstBuffer The buffer to copy to
			 * @param bufferSize Size of the buffers
			 */
			static void copyBuffer(VkPhysicalDevice &physicalDevice, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize) {
				// Create a temporary command buffer
				CommandBuffer commandBuffer {false, VK_COMMAND_BUFFER_LEVEL_PRIMARY};
				commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

				// Copy buffer
				VkBufferCopy copyRegion {};
				copyRegion.srcOffset = 0;  // Optional
				copyRegion.dstOffset = 0;  // Optional
				copyRegion.size = bufferSize;
				vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

				// Stop and execute command buffer
				commandBuffer.end();
				commandBuffer.submit();

				// Wait for command buffer queue to end operations
				commandBuffer.waitForQueueIdle();
			}

			static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
				// Create a temporary command buffer
				CommandBuffer commandBuffer {false, VK_COMMAND_BUFFER_LEVEL_PRIMARY};
				commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

				// Create an image region to copy to
				VkBufferImageCopy region {};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.mipLevel = 0;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;
				region.imageOffset = {0, 0, 0};
				region.imageExtent = {width, height, 1}; // Use the full image size

				// Copy buffer to image region
				vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

				// Stop and execute command buffer
				commandBuffer.end();
				commandBuffer.submit();

				// Wait for command buffer queue to end operations
				commandBuffer.waitForQueueIdle();
			}



			/**
			 * Finds the memory type of the given physical device
			 * @param physicalDevice
			 * @param typeFilter The memory requirements
			 * @param properties The memory requested properties
			 * @return the memory ID
			 */
			static uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
                WDE_PROFILE_FUNCTION();
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
