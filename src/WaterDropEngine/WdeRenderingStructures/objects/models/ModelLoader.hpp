#pragma once

#include "../Model.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>

namespace wde::renderStructures {
	class ModelLoader : public Model {
		public:
			explicit ModelLoader(const std::string& path) : Model() {
				// Load model
				tinyobj::attrib_t attrib;
				std::vector<tinyobj::shape_t> shapes;
				std::vector<tinyobj::material_t> materials;
				std::string warn, err;

				Logger::debug("Loading model from " + path + ".", LoggerChannel::RENDERING_STRUCTURES);
				if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
					throw WdeException("Failed to load model. " + std::string(warn) + std::string(err), LoggerChannel::RENDERING_STRUCTURES);

				// Combine every face into a single model and delete vertices repetition
				std::unordered_map<size_t, bool> hasVerticeHash {};
				std::hash<Vertex> hasher;

				for (const auto& shape : shapes) {
					for (const auto& index : shape.mesh.indices) {
						// Get vertex
						Vertex v {
							{ // position
								attrib.vertices[3 * index.vertex_index + 0],
								attrib.vertices[3 * index.vertex_index + 1],
								attrib.vertices[3 * index.vertex_index + 2]
							},
							{1.0f, 1.0f, 1.0f} // color
						};

						// Avoid vertices duplication
						size_t hash = hasher(v);
						if (!hasVerticeHash.contains(hash)) {
							_vertices.push_back(v);
							hasVerticeHash[hash] = true;
						}

						// Add index
						_indices.push_back(_indices.size());
					}
				}


				// Initialize model
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
