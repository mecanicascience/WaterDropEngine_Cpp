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
			void initialize();

			// Render functions
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
             * @param gameObjectID (default = 0)
             */
            void render(uint32_t gameObjectID = 0) {
	            WDE_PROFILE_FUNCTION();
                // Add the draw command to the command buffer
                vkCmdDrawIndexed(*_commandBuffer, _indexCount, 1, 0, 0, gameObjectID);
		    }

		    /**
		     * @param gameObjectID
		     * @return Return the rendering command for this object
		     */
	        VkDrawIndexedIndirectCommand getRenderIndirectCommand(uint32_t gameObjectID) {
		        VkDrawIndexedIndirectCommand cmd {};
		        cmd.firstIndex = 0;
		        cmd.vertexOffset = 0;
		        cmd.indexCount = _indexCount;
		        cmd.instanceCount = 1;
		        cmd.firstInstance = gameObjectID;
				return cmd;
			}

		    // Getters and setters
		    std::string getName() { return _name; }
		    int getIndexCount() { return static_cast<int>(_indexCount); }
	        void setIndexCount(uint32_t count) { _indexCount = count; }
	        std::vector<Vertex>& getVertices() { return _vertices; }
	        std::vector<uint32_t>& getIndices() { return _indices; }
	        std::shared_ptr<render::Buffer>& getVertexBuffer() { return _vertexBuffer; }
	        std::shared_ptr<render::Buffer>& getIndexBuffer() { return _indexBuffer; }


	   protected:
		   // Core
		   /** Name of the mesh */
            std::string _name;
            /** List of the mesh vertices */
            std::vector<Vertex> _vertices;
            /** List of the mesh indices */
            std::vector<uint32_t> _indices;
			/** Number of indices */
			uint32_t _indexCount;

            // Model buffers
            std::shared_ptr<render::Buffer> _indexBuffer;
            std::shared_ptr<render::Buffer> _vertexBuffer;

            // Utils
            /** Temporary reference to the render command buffer */
            render::CommandBuffer* _commandBuffer = nullptr;


		    // Core functions
		    /** Initialize the Mesh */
		    virtual void loadData() {};
		    /** Recompute the normals of each vertex */
		    void recalculateNormals();
    };
}
