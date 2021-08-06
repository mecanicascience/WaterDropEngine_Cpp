#pragma once

#include "../Model.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>

namespace wde::scene {
	class ModelLoader : public Model {
		public:
			explicit ModelLoader(const std::string& path) : Model() {
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

				for (const auto& shape : shapes) {
					for (const auto& index : shape.mesh.indices) {
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
							{0.898f, 0.149f, 1.0f}
						};
						v._color = v._normal;

						// Avoid vertices duplication
						size_t hash = hasher(v);
						if (!verticesIndexHash.contains(hash)) { // New vertex
							uint32_t indexID = _vertices.size();
							_vertices.push_back(v);

							verticesIndexHash[hash] = indexID;
							_indices.push_back(indexID);
						}
						else { // Vertex already exists
							_indices.push_back(verticesIndexHash.at(hash));
						}
					}
				}

				// Initialize model
				Logger::debug("Initializing model.", LoggerChannel::SCENE);
				this->initialize();
			};


		protected:
			// Model description
			std::vector<Vertex> getVertices() override { return _vertices; }
			std::vector<uint32_t> getIndices() override { return _indices; }


		private:
			std::vector<Vertex> _vertices {};
			std::vector<uint32_t> _indices {};
	};
}
