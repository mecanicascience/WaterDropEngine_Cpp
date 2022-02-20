#pragma once
#include "../../../../wde.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/matrix_transform.hpp>

namespace wde::scene {
    /**
     * Generic class that stores a vertex vulkan description for the shaders
     */
    struct VertexInput {
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


    /**
     * Represents a mesh unique vertex
     */
    struct Vertex {
        // Vertex data
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
		glm::vec2 uv;

        /**
         * Convert this vertex to a vulkan-compatible shader description format
         * @param baseBinding Initial binding offset of these vertices in the buffer (default 0)
         * @return The formatted general VertexInput description of a vertex
         */
        static VertexInput getDescriptions(uint32_t baseBinding = 0) {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
                    {baseBinding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
            };
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
                    {0, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)}, // Vertex position values (index 0)
                    {1, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)}, // Normals position values (index 1)
                    {2, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},  // Vertex color values (index 2)
                    {3, baseBinding, VK_FORMAT_R32G32_SFLOAT   , offsetof(Vertex, uv)}     // UV texture coords (index 3)
            };
            return { bindingDescriptions, attributeDescriptions };
        }
    };
}

namespace std {
	/**
	 * Create a hash function for a vertex instance
	 */
	template<> struct hash<wde::scene::Vertex> {
		size_t operator()(wde::scene::Vertex const& vertex) const {
			return    (((hash<glm::vec3>()(vertex.position)
			             ^ (hash<glm::vec3>()(vertex.color) << 1) >> 1)
			            ^ (hash<glm::vec2>()(vertex.uv) << 1)) >> 1)
			          ^ (hash<glm::vec3>()(vertex.normal) << 1);
		}
	};
}