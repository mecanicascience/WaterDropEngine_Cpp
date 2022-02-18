#include "Mesh.hpp"

namespace wde::scene {
    Mesh::Mesh(std::string name) : _name(std::move(name)) {
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


        // Create buffers
        // TODO
    }

    Mesh::~Mesh() {
        _commandBuffer = nullptr;
    }
}
