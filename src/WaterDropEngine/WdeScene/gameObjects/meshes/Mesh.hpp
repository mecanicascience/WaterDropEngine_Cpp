#pragma once
#include <utility>

#include "../../../../wde.hpp"
#include "Vertex.hpp"
#include "../../../WdeRender/commands/CommandBuffer.hpp"
#include "../../../WdeRender/buffers/Buffer.hpp"

namespace wde::scene {
    /**
     * Handles a game mesh
     */
    class Mesh {
        public:
            explicit Mesh(std::string name);
            ~Mesh();

            // Core functions
            /**
             * Bind the mesh to the command buffer
             * @param commandBuffer
             */
            void bind(render::CommandBuffer& commandBuffer) {
	            WDE_PROFILE_FUNCTION();
                _commandBuffer = &commandBuffer;

                // Bind our vertex into the commandBuffer with index of offsets[]
                VkBuffer vertexBuffers[] = { _vertexBuffer->getBuffer() };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

                // Bind index buffers into the commandBuffer with index of offsets[]
                vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32); // VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32
            }

            /**
             * Render the given game object using this mesh
             * @param gameObjectID
             */
            void render(uint32_t gameObjectID) {
	            WDE_PROFILE_FUNCTION();
                // Add the draw command to the command buffer
                vkCmdDrawIndexed(*_commandBuffer, _indices.size(), 1, 0, 0, gameObjectID);
		   }

		   // Getters and setters
		   std::string getName() { return _name; }


	   private:
		   // Core
		   /** Name of the mesh */
            std::string _name;
            /** List of the mesh vertices */
            std::vector<Vertex> _vertices;
            /** List of the mesh indices */
            std::vector<uint32_t> _indices;

            // Model buffers
            std::unique_ptr<render::Buffer> _indexBuffer;
            std::unique_ptr<render::Buffer> _vertexBuffer;

            // Utils
            /** Temporary reference to the render command buffer */
            render::CommandBuffer* _commandBuffer = nullptr;
    };
}
