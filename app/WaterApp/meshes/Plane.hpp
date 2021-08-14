#pragma once

#include <WaterDropEngine/includes.hpp>


class Plane : public Model {
	public:
		/**
		 * Creates a 1x1 unit square given the subdivision number on the XZ plane
		 * @param sizeX Number of subdivisions on the X axis (default = 10)
		 * @param sizeY Number of subdivisions on the Y axis (default = 10)
		 */
		explicit Plane(int sizeX = 10, int sizeY = 10) : Model() {
			// Create vertices and indices
			float xStep = 1.0f / ((float) (sizeX + 1));
			float yStep = 1.0f / ((float) (sizeY + 1));

			// To center plane at (0, 0)
			float offsetX = 0.5f;
			float offsetY = 0.5f;

			for (int y = 0; y < sizeY + 1; y++) {
				for (int x = 0; x < sizeX + 1; x++) {
					// Create vertex at (x, y)
					float xPos = xStep * ((float) x);
					float yPos = yStep * ((float) y);
					_vertices.push_back({ // Index : x + y * (sizeY - 1)
						{xPos - offsetX, 0, yPos - offsetY}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {xPos, yPos}
					});

					// Create indices
					if (x != sizeX && y != sizeY) {
						int currentIndex = x + y * (sizeY + 1);

						// First triangle
						_indices.push_back(currentIndex + 1);
						_indices.push_back(currentIndex);
						_indices.push_back(currentIndex + (sizeY + 1));

						// Second triangle
						_indices.push_back(currentIndex + (sizeY + 1));
						_indices.push_back(currentIndex + (sizeY + 2));
						_indices.push_back(currentIndex + 1);
					}
				}
			}

			// Initialize model
			this->initialize(false);
		};


	protected:
		// Model description
		std::vector<Vertex> getVertices() override { return _vertices; }
		std::vector<uint32_t> getIndices() override { return _indices; }


	private:
		std::vector<Vertex> _vertices {};
		std::vector<uint32_t> _indices {};
};

