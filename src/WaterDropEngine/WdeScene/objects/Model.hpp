#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../WdeRenderingEngine/commands/CommandBuffer.hpp"
#include "../../WdeRenderingEngine/buffers/BufferUtils.hpp"

using namespace wde::renderEngine;

namespace wde::scene {
	class Model {
		public:
			/**
			 * Generic class used to define sets of vertex inputs used in a shader
			 */
			class VertexInput {
			public:
				VertexInput(std::vector<VkVertexInputBindingDescription> bindingDescriptions, std::vector<VkVertexInputAttributeDescription> attributeDescriptions)
				: _bindingDescriptions(std::move(bindingDescriptions)), _attributeDescriptions(std::move(attributeDescriptions)) { }

				const std::vector<VkVertexInputBindingDescription> &getBindingDescriptions() const { return _bindingDescriptions; }
				const std::vector<VkVertexInputAttributeDescription> &getAttributeDescriptions() const { return _attributeDescriptions; }

				bool operator<(const VertexInput &rhs) const {
					return _bindingDescriptions.front().binding < rhs._bindingDescriptions.front().binding;
				}

			private:
				std::vector<VkVertexInputBindingDescription> _bindingDescriptions;
				std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;
			};


			// Structures
			/**
			 * Represent a vertex. Data will be passed to the shaders.
			 */
			struct Vertex {
				public:
					/**
					 * Creates a new vertex
					 * @param position
					 * @param normal
					 * @param color
					 * @param uv
					 */
					Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color, glm::vec2 uv)
						: _position(position), _normal(normal), _color(color), _uv(uv) { }

					static VertexInput getDescriptions(uint32_t baseBinding = 0) {
						std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
								{baseBinding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
						};
						std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
								{0, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _position)}, // Vertex position values (index 0)
								{1, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _normal)}, // Normals position values (index 1)
								{2, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, _color)},  // Vertex color values (index 2)
								{3, baseBinding, VK_FORMAT_R32G32_SFLOAT   , offsetof(Vertex, _uv)}     // UV texture coords (index 3)
						};
						return { bindingDescriptions, attributeDescriptions };
					}


					// Getters and setters
					const glm::vec3& getPosition() const { return _position; }
					const glm::vec3& getColor() const { return _color; }
					const glm::vec3& getNormal() const { return _normal; }
					const glm::vec2& getUV() const { return _uv; }
					void setNormal(glm::vec3 normal) { _normal = normal; }

					// Model values
					glm::vec3 _position;
					glm::vec3 _normal;
					glm::vec3 _color;
					glm::vec2 _uv;
			};



			// Constructors
			/** Constructs a new model given a mesh */
			explicit Model() = default;
			~Model() = default;


			// Core functions
			/**
			 * Initialize the model
			 * @param recalculateNormals True will recalculate every model normals
			 */
			void initialize(bool recalculateNormals = false);

			/**
			 * Binds the model to the provided command buffer
			 * @param commandBuffer
			 */
			void bind(CommandBuffer &commandBuffer);
			/** Renders the model to the provided command buffer */
			void render();
			/** Clean up the model */
			void cleanUp();

			virtual std::vector<Vertex> getVertices() = 0;
			virtual std::vector<uint32_t> getIndices() = 0;



		private:
			// Vertex and index count
			/** The number of vertices in the model */
			uint32_t _vertexCount;
			/** The number of indices in the model */
			uint32_t _indexCount;

			// Vertex buffer
			/** The vertex buffer containing vertex of the model */
			VkBuffer _vertexBuffer;
			/** The memory used by the vertex buffer */
			VkDeviceMemory _vertexBufferMemory;

			// Index buffer
			/** The idex buffer containing indices of the model */
			VkBuffer _indexBuffer;
			/** The memory used by the index buffer */
			VkDeviceMemory _indexBufferMemory;

			/** The last binded command buffer */
			CommandBuffer *_commandBuffer = nullptr;


			// Core functions
			void createVertexBuffers(const std::vector<Vertex> &vertices);
			void createIndexBuffers(const std::vector<uint32_t> &indices);

			// Helper functions
			/** Compute the normals of each vertex */
			static void recalculateModelNormals(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);
	};
}

namespace std {
	/**
	 * Create a hash function for a vertex instance
	 */
	template<> struct hash<wde::scene::Model::Vertex> {
		size_t operator()(wde::scene::Model::Vertex const& vertex) const {
			return    (((hash<glm::vec3>()(vertex.getPosition())
			        ^ (hash<glm::vec3>()(vertex.getColor()) << 1) >> 1)
			        ^ (hash<glm::vec2>()(vertex.getUV()) << 1)) >> 1)
			        ^ (hash<glm::vec3>()(vertex.getNormal()) << 1);
		}
	};
}
