#include "Mesh.hpp"

namespace wde::scene {
    Mesh::Mesh(std::string name) : _name(std::move(name)) {}

	void Mesh::initialize() {
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Initializing mesh." << logger::endl;
	    {
		    WDE_PROFILE_SCOPE("wde::scene::Mesh::createVerticesBuffer");
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
		    WDE_PROFILE_SCOPE("wde::scene::Mesh::createIndicesBuffer");
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


	void Mesh::recalculateNormals() {
		WDE_PROFILE_FUNCTION();
		logger::log << "Recalculating model normals." << logger::endl;

		// Reset vertices normals
		for (auto& vertex : _vertices)
			vertex.normal = glm::vec3 {0.0f, 0.0f, 0.0f};

		// For each triangle, add the triangle normal to the binding vertices
		for (int i = 0; i < _indices.size(); i += 3) {
			// Triangle A,B,C
			Vertex& a = _vertices[_indices[i + 0]];
			Vertex& b = _vertices[_indices[i + 1]];
			Vertex& c = _vertices[_indices[i + 2]];

			// Vectors
			glm::vec3 ab = b.position - a.position;
			glm::vec3 ac = c.position - a.position;

			// Calculate normal as AB x AC (assuming anti-clockwise normal orientation)
			glm::vec3 n = glm::cross(ab, ac);

			// Add normal
			a.normal += n;
			b.normal += n;
			c.normal += n;
		}

		// Normalize each vertex
		for (auto& vertex : _vertices)
			vertex.normal = glm::normalize(vertex.normal);
	}
}
