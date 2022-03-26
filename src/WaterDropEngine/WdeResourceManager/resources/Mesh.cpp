#include "Mesh.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::resource {
	Mesh::Mesh(const std::string &path) : Resource(path, ResourceType::MESH) {
		WDE_PROFILE_FUNCTION();
		auto matData = json::parse(WdeFileUtils::readFile(path));
		if (matData["type"] != "mesh")
			throw WdeException(LogChannel::RES, "Trying to load a mesh from a non-mesh description.");

		// Data
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		_name = matData["name"];

		// Load data
		{
			WDE_PROFILE_SCOPE("wde::scene::Mesh::loadMesh");
			std::string resPath = WaterDropEngine::get().getInstance().getScene().getPath() + "data/meshes/" + matData["data"]["path"].get<std::string>();

			// Load model
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn, err;

			logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Loading model from " << resPath << "." << logger::endl;
			tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, resPath.c_str());
			if (!warn.empty())
				logger::log(LogLevel::WARN, LogChannel::SCENE) << "Failed to load model. " + std::string(warn) + std::string(err) << logger::endl;
			if (!err.empty())
				throw WdeException(LogChannel::SCENE, "Failed to load model. " + std::string(warn) + std::string(err));

			// Combine every face into a single model and delete vertices repetition
			std::unordered_map<size_t, uint32_t> verticesIndexHash {};
			std::hash<Vertex> hasher;

			// Loop over shapes
			_occlusionSphere.w = 0.0;
			for (const auto& shape : shapes) {
				size_t faceTriangleOffset = 0; // Offset of the current triangle in the shape

				// Loop over faces in the shape
				for (size_t faceIndex = 0; faceIndex < shape.mesh.num_face_vertices.size(); faceIndex++) {
					int faceSize = 3; // Load triangles (3 vertices)

					// Loop over vertices in the face
					for (size_t vertexID = 0; vertexID < faceSize; vertexID++) {
						// Access to vertex
						tinyobj::index_t index = shape.mesh.indices[faceTriangleOffset + vertexID];

						// Set occlusion box bounds
						auto vLength = std::sqrt(
								attrib.vertices[3 * index.vertex_index + 0] * attrib.vertices[3 * index.vertex_index + 0]
								+ attrib.vertices[3 * index.vertex_index + 1] * attrib.vertices[3 * index.vertex_index + 1]
								+ attrib.vertices[3 * index.vertex_index + 2] * attrib.vertices[3 * index.vertex_index + 2]);
						if (vLength * 2.0 > _occlusionSphere.w)
							_occlusionSphere.w = vLength * 2;

						// Get vertex
						Vertex v {
							{
								attrib.vertices[3 * index.vertex_index + 0],
								attrib.vertices[3 * index.vertex_index + 1],
								attrib.vertices[3 * index.vertex_index + 2]
							},
							{
								attrib.normals[3 * index.normal_index + 0],
								attrib.normals[3 * index.normal_index + 1],
								attrib.normals[3 * index.normal_index + 2]
							},
							{
								attrib.texcoords[2 * index.texcoord_index + 0],
								attrib.texcoords[2 * index.texcoord_index + 1]
							}
						};
						// v._color = v._normal; // Use normals as color
						v.uv.y = 1.0f - v.uv.y; // Invert uvs (they work as inverted in Vulkan)

						// Avoid vertices duplication
						size_t hash = hasher(v);
						if (!verticesIndexHash.contains(hash)) { // New vertex
							uint32_t indexID = vertices.size();
							vertices.push_back(v); // push vertices

							verticesIndexHash[hash] = indexID;
							indices.push_back(indexID); // push indices
						}
						else // Vertex already exists
							indices.push_back(verticesIndexHash.at(hash));
					}

					faceTriangleOffset += faceSize;
				}
			}
		}

		// Recalculate normals
		if (matData["data"]["recalculateNormals"].get<bool>()) {
			WDE_PROFILE_SCOPE("wde::scene::Mesh::recomputeNormals");
			logger::log << "Recalculating model normals." << logger::endl;

			// Reset vertices normals
			for (auto& vertex : vertices)
				vertex.normal = glm::vec3 {0.0f, 0.0f, 0.0f};

			// For each triangle, add the triangle normal to the binding vertices
			for (int i = 0; i < _indexCount; i += 3) {
				// Triangle A,B,C
				Vertex& a = vertices[indices[i + 0]];
				Vertex& b = vertices[indices[i + 1]];
				Vertex& c = vertices[indices[i + 2]];

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
			for (auto& vertex : vertices)
				vertex.normal = glm::normalize(vertex.normal);
		}

		// Initialize mesh
		{
			WDE_PROFILE_SCOPE("wde::scene::Mesh::createVerticesBuffer");
			// Assert that vertices count >= 3
			if (vertices.size() < 3)
				return;

			// Create vertex transfer buffer
			VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
			render::Buffer stagingBuffer {
					bufferSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Transfer buffer from CPU to GPU
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT}; // CPU writes to GPU

			// Copy the vertices buffer to the staging buffer
			void *data = stagingBuffer.map();
			memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
			stagingBuffer.unmap();

			// Create vertex buffer
			_vertexBuffer = std::make_shared<render::Buffer>(
					bufferSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Use this buffer as a destination on the GPU
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // Use most efficient memory possible

			// Transfer data from staging to vertex buffer
			stagingBuffer.copyTo(*_vertexBuffer);
		}

		// Create index buffer
		{
			WDE_PROFILE_SCOPE("wde::scene::Mesh::createIndicesBuffer");
			// Set indices count
			if (!indices.empty())
				_indexCount = indices.size();

			// Assert that indices count >= 3
			if (_indexCount < 3)
				return;

			// Create index transfer buffer
			VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;
			render::Buffer stagingBuffer {
					bufferSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Transfer buffer from CPU to GPU
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT}; // CPU writes to GPU

			// Copy the indices buffer to the staging buffer
			void *data = stagingBuffer.map();
			memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
			stagingBuffer.unmap();

			// Create index buffer
			_indexBuffer = std::make_shared<render::Buffer>(
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




	void Mesh::bind(render::CommandBuffer &commandBuffer)  {
		WDE_PROFILE_FUNCTION();
		_commandBuffer = &commandBuffer;

		// Bind our vertex into the commandBuffer with index of offsets[]
		VkBuffer vertexBuffers[] = { _vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		// Bind index buffers into the commandBuffer with index of offsets[]
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32); // VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32
	}

	void Mesh::render(uint32_t gameObjectID) {
		WDE_PROFILE_FUNCTION();
		// Add the draw command to the command buffer
		vkCmdDrawIndexed(*_commandBuffer, _indexCount, 1, 0, 0, gameObjectID);
	}
}
