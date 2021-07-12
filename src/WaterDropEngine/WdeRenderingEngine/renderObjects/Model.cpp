#include "Model.hpp"

namespace wde::renderEngine {
	Model::Model(VkDevice &device, VkPhysicalDevice &physicalDevice, const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
		// Create buffers
		Logger::debug("Creating vertex and index buffers.", LoggerChannel::RENDERING_ENGINE);
		createVertexBuffers(device, physicalDevice, vertices);
		createIndexBuffers(device, physicalDevice, indices);
	}

	void Model::cleanUp(VkDevice &device) {
		// Destroy vertex buffers
		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);

		// Destroy index buffers
		vkDestroyBuffer(device, indexBuffer, nullptr);
		vkFreeMemory(device, indexBufferMemory, nullptr);
	}



	void Model::createVertexBuffers(VkDevice &device, VkPhysicalDevice &physicalDevice, const std::vector<Vertex> &vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		if (vertexCount < 3)
			throw WdeException("Vertex count must be at least 3.", LoggerChannel::RENDERING_ENGINE);

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		RenderingUtils::createBuffer(physicalDevice, device,
				bufferSize,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				vertexBuffer,
				vertexBufferMemory);

		void *data;
		vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device, vertexBufferMemory);
	}

	void Model::createIndexBuffers(VkDevice &device, VkPhysicalDevice &physicalDevice, const std::vector<uint16_t> &indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		if (vertexCount < 3)
			throw WdeException("Vertex count must be at least 3.", LoggerChannel::RENDERING_ENGINE);

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		RenderingUtils::createBuffer(physicalDevice, device,
		                             bufferSize,
		                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		                             indexBuffer,
		                             indexBufferMemory);

		void *data;
		vkMapMemory(device, indexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device, indexBufferMemory);
	}




	void Model::bind(VkCommandBuffer commandBuffer) {
		// Bind our vertex into the commandBuffer with index of offsets[]
		VkBuffer vertexBuffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		// Bind index buffers into the commandBuffer with index of offsets[]
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16); // VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32
	}

	void Model::draw(VkCommandBuffer commandBuffer) {
		// Draw as vertexBuffer[indexBuffer[i]] on each buffer
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}





	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
		// We have a vertex buffer
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::array<VkVertexInputAttributeDescription, 2> Model::Vertex::getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

		// Vertex position values (at index 0)
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		// Vertex color values (at index 1)
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
}
