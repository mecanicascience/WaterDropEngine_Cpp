#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <vulkan/vulkan_core.h>

#include "../../WdeCommon/WdeError/WdeException.hpp"
#include "../utils/RenderingUtils.hpp"

namespace wde::renderEngine {
	class Model {
		public:
			// Structures
			struct Vertex {
				glm::vec2 position;
				glm::vec3 color;

				static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
				static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
			};


			// Constructors
			Model(VkDevice &device, VkPhysicalDevice &physicalDevice, const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices);
			~Model() = default;

			void cleanUp(VkDevice &device);

			// Avoid copys
			Model(const Model &) = delete;
			Model &operator=(const Model &) = delete;


			// Core functions
			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);


		private:
			/** The vertex buffer containing vertex of the model */
			VkBuffer vertexBuffer;
			/** The memory used by the vertex buffer */
			VkDeviceMemory vertexBufferMemory;

			/** The index buffer containing indices of the model */
			VkBuffer indexBuffer;
			/** The memory used by the index buffer */
			VkDeviceMemory indexBufferMemory;

			/** The number of vertices in the model */
			uint32_t vertexCount;
			/** The number of indices in the model */
			uint32_t indexCount;


			// Core functions
			void createVertexBuffers(VkDevice &device, VkPhysicalDevice &physicalDevice, const std::vector<Vertex> &vertices);
			void createIndexBuffers(VkDevice &device, VkPhysicalDevice &physicalDevice, const std::vector<uint16_t> &indices);
	};
}
