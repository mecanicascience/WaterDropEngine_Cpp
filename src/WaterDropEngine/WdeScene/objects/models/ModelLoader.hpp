#pragma once

#include "../Model.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>

namespace wde::scene {
	class ModelLoader : public Model {
		public:
			/**
			 * Loads a new model
			 * @param relativePath The relative path of the object (final path will be "res/models/your_path")
			 * @param recalculateNormals True if the program needs to recalculate model normals (default : false)
			 */
			explicit ModelLoader(const std::string& relativePath, bool recalculateNormals = false) : _relativePath(relativePath), Model() {
				std::string path = "res/models/" + _relativePath;

				// Load model
				tinyobj::attrib_t attrib;
				std::vector<tinyobj::shape_t> shapes;
				std::vector<tinyobj::material_t> materials;
				std::string warn, err;

				Logger::debug("Loading model from " + path + ".", LoggerChannel::SCENE);
				tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
				if (!warn.empty())
					Logger::warn("Failed to load model. " + std::string(warn) + std::string(err), LoggerChannel::SCENE);
				if (!err.empty())
					throw WdeException("Failed to load model. " + std::string(warn) + std::string(err), LoggerChannel::SCENE);

				// Combine every face into a single model and delete vertices repetition
				std::unordered_map<size_t, uint32_t> verticesIndexHash {};
				std::hash<Vertex> hasher;

				// Loop over shapes
				for (const auto& shape : shapes) {
					size_t faceTriangleOffset = 0; // Offset of the current triangle in the shape

					// Loop over faces in the shape
					for (size_t faceIndex = 0; faceIndex < shape.mesh.num_face_vertices.size(); faceIndex++) {
						int faceSize = 3; // Load triangles (3 vertices)

						// Loop over vertices in the face
						for (size_t vertexID = 0; vertexID < faceSize; vertexID++) {
							// Access to vertex
							tinyobj::index_t index = shape.mesh.indices[faceTriangleOffset + vertexID];

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
									{0.898f, 0.149f, 1.0f}, // Undefined texture (pink color)
								{
									attrib.texcoords[2 * index.texcoord_index + 0],
									attrib.texcoords[2 * index.texcoord_index + 1]
								}
							};
							// v._color = v._normal; // Use normals as color
							v._uv.y = 1.0f - v._uv.y; // Invert uvs (work as inverted in Vulkan)

							// Avoid vertices duplication
							size_t hash = hasher(v);
							if (!verticesIndexHash.contains(hash)) { // New vertex
								uint32_t indexID = _vertices.size();
								_vertices.push_back(v); // push vertices

								verticesIndexHash[hash] = indexID;
								_indices.push_back(indexID); // push indices
							}
							else { // Vertex already exists
								_indices.push_back(verticesIndexHash.at(hash));
							}
						}

						faceTriangleOffset += faceSize;
					}
				}

				// Initialize model
				Logger::debug("Initializing model.", LoggerChannel::SCENE);
				this->initialize(recalculateNormals); // Assume that model normals are correct
			};

			// Serializers
			json serialize() override {
				return json({
					{"type", "external"},
					{"relativePath", _relativePath}
				});
			}


		protected:
			// Model description
			std::vector<Vertex> getVertices() override { return _vertices; }
			std::vector<uint32_t> getIndices() override { return _indices; }


		private:
			std::string _relativePath;

			std::vector<Vertex> _vertices {};
			std::vector<uint32_t> _indices {};
	};
}
