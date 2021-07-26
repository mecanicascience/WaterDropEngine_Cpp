#include "Model.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
	Model::Model(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
        WDE_PROFILE_FUNCTION();
		// Set the vertices
		createVertexBuffers(vertices);

		// Set the indices
		createIndexBuffers(indices);
	}

	Model::~Model() {
        WDE_PROFILE_FUNCTION();
		auto &device = CoreInstance::get().getSelectedDevice().getDevice();

		// Destroy vertex buffers
		vkDestroyBuffer(device, _vertexBuffer, nullptr);
		vkFreeMemory(device, _vertexBufferMemory, nullptr);

		// Destroy index buffers
		vkDestroyBuffer(device, _indexBuffer, nullptr);
		vkFreeMemory(device, _indexBufferMemory, nullptr);
	}


	// Core functions
	void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
        WDE_PROFILE_FUNCTION();
		auto &device = CoreInstance::get().getSelectedDevice().getDevice();

		_vertexCount = static_cast<uint32_t>(vertices.size());
		if (_vertexCount < 3)
			throw WdeException("Vertex count must be at least 3.", LoggerChannel::RENDERING_ENGINE);

		VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;
		BufferUtils::createBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(),
                                  CoreInstance::get().getSelectedDevice().getDevice(), bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  _vertexBuffer, _vertexBufferMemory);

		void *data;
		vkMapMemory(device, _vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device, _vertexBufferMemory);
	}

	void Model::createIndexBuffers(const std::vector<uint16_t> &indices) {
        WDE_PROFILE_FUNCTION();
		auto &device = CoreInstance::get().getSelectedDevice().getDevice();

		_indexCount = static_cast<uint32_t>(indices.size());
		if (_indexCount < 3)
			throw WdeException("Index count must be at least 3.", LoggerChannel::RENDERING_ENGINE);

		VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;
        BufferUtils::createBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(),
                                  CoreInstance::get().getSelectedDevice().getDevice(), bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  _indexBuffer, _indexBufferMemory);

		void *data;
		vkMapMemory(device, _indexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device, _indexBufferMemory);
	}


	void Model::render(const CommandBuffer &commandBuffer) {
        WDE_PROFILE_FUNCTION();
		// Bind our vertex into the commandBuffer with index of offsets[]
		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		// Bind index buffers into the commandBuffer with index of offsets[]
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16); // VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32

		// Add the draw command to the command buffer
		vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
	}
}
