#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../commands/CommandBuffer.hpp"
#include "../buffers/BufferUtils.hpp"

namespace wde::renderEngine {
	class Model {
		public:
			/**
			 * Class used to define sets of vertex inputs used in a shader
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
			struct Vertex {
				glm::vec2 position;
				glm::vec3 color;

				static VertexInput getDescriptions(uint32_t baseBinding = 0) {
					std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
							{baseBinding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
					};
					std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
							{0, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)}, // Vertex position values (at index 0)
							{1, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)}  // Vertex color values (at index 1)
					};
					return { bindingDescriptions, attributeDescriptions };
				}
			};


			// Constructors
			/**
			 * Constructs a new model given the vertices and indices
			 * @param vertices
			 * @param indices
			 */
            explicit Model(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices);
			~Model();


			// Core functions
			/**
			 * Renders the model to the provided command buffer
			 * @param commandBuffer
			 */
			void render(const CommandBuffer &commandBuffer);


		private:
			/** The number of vertices in the model */
			uint32_t _vertexCount;
			/** The number of indices in the model */
			uint32_t _indexCount;

			/** The vertex buffer containing vertex of the model */
			VkBuffer _vertexBuffer;
			/** The memory used by the vertex buffer */
			VkDeviceMemory _vertexBufferMemory;

			/** The index buffer containing indices of the model */
			VkBuffer _indexBuffer;
			/** The memory used by the index buffer */
			VkDeviceMemory _indexBufferMemory;


			// Core functions
			void createVertexBuffers(const std::vector<Vertex> &vertices);
			void createIndexBuffers(const std::vector<uint16_t> &indices);
	};
}
