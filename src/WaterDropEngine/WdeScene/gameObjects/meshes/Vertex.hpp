#pragma once
#include "../../../../wde.hpp"

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
            };
            return { bindingDescriptions, attributeDescriptions };
        }
    };
}