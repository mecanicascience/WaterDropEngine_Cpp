#include "Model.hpp"
#include "../../WdeRenderingEngine/core/CoreInstance.hpp"

namespace wde::renderStructures {
	Model::Model() { }

	Model::~Model() {
        WDE_PROFILE_FUNCTION();
		auto &device = CoreInstance::get().getSelectedDevice().getDevice();

		// Destroy vertex buffers
		vkDestroyBuffer(device, _vertexBuffer, nullptr);
		vkFreeMemory(device, _vertexBufferMemory, nullptr);

		// Destroy index buffers
		vkDestroyBuffer(device, _indexBuffer, nullptr);
		vkFreeMemory(device, _indexBufferMemory, nullptr);

		// Delete command buffer reference
		_commandBuffer = nullptr;
	}



	// Core functions
	void Model::initialize() {
		WDE_PROFILE_FUNCTION();
		// Set the vertices
		createVertexBuffers(getVertices());

		// Set the indices
		createIndexBuffers(getIndices());
	}


	void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
        WDE_PROFILE_FUNCTION();
		auto &device = CoreInstance::get().getSelectedDevice().getDevice();

		// Assert that vertices count >= 3
		_vertexCount = static_cast<uint32_t>(vertices.size());
		if (_vertexCount < 3)
			throw WdeException("Vertex count must be at least 3.", LoggerChannel::RENDERING_STRUCTURES);


		// Create the staging buffer on the CPU
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;
		BufferUtils::createBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(),
                                  CoreInstance::get().getSelectedDevice().getDevice(), bufferSize,
                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Transfer buffer from CPU to GPU
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // CPU writes to GPU
                                  stagingBuffer, stagingBufferMemory);

		// Copy the vertices buffer to the staging buffer
		void *data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device, stagingBufferMemory);

		// Copy the staging buffer to the local GPU memory
		BufferUtils::createBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(),
								  CoreInstance::get().getSelectedDevice().getDevice(), bufferSize,
								  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Use this buffer as a destination on the GPU
								  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // Use most efficient memory possible
								  _vertexBuffer, _vertexBufferMemory);
		BufferUtils::copyBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(), stagingBuffer, _vertexBuffer, bufferSize);

		// Clean up staging buffer
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
        WDE_PROFILE_FUNCTION();
		auto &device = CoreInstance::get().getSelectedDevice().getDevice();

		// Assert that indices count >= 3
		_indexCount = static_cast<uint32_t>(indices.size());
		if (_indexCount < 3)
			throw WdeException("Index count must be at least 3.", LoggerChannel::RENDERING_STRUCTURES);

		// Create the staging buffer on the CPU
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;
		BufferUtils::createBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(),
								  CoreInstance::get().getSelectedDevice().getDevice(), bufferSize,
								  VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Transfer buffer from CPU to GPU
								  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // CPU writes to GPU
								  stagingBuffer, stagingBufferMemory);

		// Copy the indices buffer to the staging buffer
		void *data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device, stagingBufferMemory);

		// Copy the staging buffer to the local GPU memory
		BufferUtils::createBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(),
								  CoreInstance::get().getSelectedDevice().getDevice(), bufferSize,
								  VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Use this buffer as a destination on the GPU
								  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // Use most efficient memory possible
								  _indexBuffer, _indexBufferMemory);
		BufferUtils::copyBuffer(CoreInstance::get().getSelectedDevice().getPhysicalDevice(), stagingBuffer, _indexBuffer, bufferSize);

		// Clean up staging buffer
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}




	void Model::bind(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		_commandBuffer = &commandBuffer;

		// Bind our vertex into the commandBuffer with index of offsets[]
		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		// Bind index buffers into the commandBuffer with index of offsets[]
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32); // VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32
	}

	void Model::render() {
        WDE_PROFILE_FUNCTION();

		// Add the draw command to the command buffer
		vkCmdDrawIndexed(*_commandBuffer, _indexCount, 1, 0, 0, 0);
	}
}
