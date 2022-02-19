#include "Mesh.hpp"

namespace wde::scene {
    Mesh::Mesh(std::string name) : _name(std::move(name)) {
	    WDE_PROFILE_FUNCTION();
        // Hardcoded vertices for now
        _vertices.resize(3);
        _vertices[0].position = { 1.f, 1.f, 0.0f };
        _vertices[1].position = {-1.f, 1.f, 0.0f };
        _vertices[2].position = { 0.f,-1.f, 0.0f };

        _vertices[0].color = { 0.f, 1.f, 0.0f }; //pure green
        _vertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
        _vertices[2].color = { 0.f, 1.f, 0.0f }; //pure green

        // Hardcoded indices for now
        _indices.resize(3);
        _indices[0] = 0;
        _indices[1] = 1;
        _indices[2] = 2;


	    {
		    // Assert that vertices count >= 3
		    if (_vertices.size() < 3)
			    throw WdeException(LogChannel::SCENE, "Vertex count must be at least 3.");

			// Create vertex transfer buffer
		    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();
			render::Buffer stagingBuffer {
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Transfer buffer from CPU to GPU
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT}; // CPU writes to GPU

		    // Copy the vertices buffer to the staging buffer
		    void *data = stagingBuffer.map();
		    memcpy(data, _vertices.data(), static_cast<size_t>(bufferSize));
		    stagingBuffer.unmap();

			// Create vertex buffer
			_vertexBuffer = std::make_unique<render::Buffer>(
					bufferSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Use this buffer as a destination on the GPU
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // Use most efficient memory possible

			// Transfer data from staging to vertex buffer
		    stagingBuffer.copyTo(*_vertexBuffer);
		}
	    {
		    // Assert that indices count >= 3
		    if (_indices.size() < 3)
			    throw WdeException(LogChannel::SCENE, "Index count must be at least 3.");

		    // Create index transfer buffer
		    VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();
		    render::Buffer stagingBuffer {
				    bufferSize,
				    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Transfer buffer from CPU to GPU
				    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT}; // CPU writes to GPU

		    // Copy the indices buffer to the staging buffer
		    void *data = stagingBuffer.map();
		    memcpy(data, _indices.data(), static_cast<size_t>(bufferSize));
		    stagingBuffer.unmap();

		    // Create index buffer
		    _indexBuffer = std::make_unique<render::Buffer>(
				    bufferSize,
				    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Use this buffer as a destination on the GPU
				    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // Use most efficient memory possible

		    // Transfer data from staging to index buffer
		    stagingBuffer.copyTo(*_indexBuffer);
		}
    }

    Mesh::~Mesh() {
        _commandBuffer = nullptr;
    }
}
