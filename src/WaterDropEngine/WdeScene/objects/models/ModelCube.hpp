#pragma once

#include "../Model.hpp"

namespace wde::scene {
	class ModelCube : public Model {
		public:
			explicit ModelCube() : Model() {
				// Recalculate model normals
				// this->recalculateNormals();

				// Initialize model
				this->initialize();
			};


		protected:
			// Model description
			std::vector<Vertex> getVertices() override {
				return {
					// left face (white)
					{{-.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
					{{-.5f, .5f, .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
					{{-.5f, -.5f, .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},
					{{-.5f, .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .9f, .9f}},

					// right face (yellow)
					{{.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
					{{.5f, .5f, .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
					{{.5f, -.5f, .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},
					{{.5f, .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .8f, .1f}},

					// top face (orange, remember y axis points down)
					{{-.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
					{{.5f, -.5f, .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
					{{-.5f, -.5f, .5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},
					{{.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.9f, .6f, .1f}},

					// bottom face (red)
					{{-.5f, .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
					{{.5f, .5f, .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
					{{-.5f, .5f, .5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
					{{.5f, .5f, -.5f}, {0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},

					// nose face (blue)
					{{-.5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
					{{.5f, .5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
					{{-.5f, .5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
					{{.5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},

					// tail face (green)
					{{-.5f, -.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
					{{.5f, .5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
					{{-.5f, .5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
					{{.5f, -.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
				};
			}

			std::vector<uint32_t> getIndices() override {
				return { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
						  12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };
			}
	};
}
