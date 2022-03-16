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
    class Mesh : public NonCopyable {
        public:
            explicit Mesh(std::string name);
            ~Mesh() override;
			void initialize();

			// Render functions
            /**
             * Bind the mesh to the command buffer
             * @param commandBuffer
             */
            void bind(render::CommandBuffer& commandBuffer);

            /**
             * Render the given game object using this mesh
             * @param gameObjectID (default = 0)
             */
            void render(uint32_t gameObjectID = 0);

		    /**
		     * @param gameObjectID
		     * @return Return the rendering command for this object
		     */
	        VkDrawIndexedIndirectCommand getRenderIndirectCommand(uint32_t gameObjectID) const {
		        VkDrawIndexedIndirectCommand cmd {};
		        cmd.firstIndex = 0;
		        cmd.vertexOffset = 0;
		        cmd.indexCount = _indexCount;
		        cmd.instanceCount = 1;
		        cmd.firstInstance = gameObjectID;
				return cmd;
			}

		    /** Recompute the normals of each vertex */
		    void recalculateNormals();

		    // Getters and setters
		    std::string getName() const { return _name; }
		    int getIndexCount() const { return static_cast<int>(_indexCount); }
	        void setIndexCount(uint32_t count) { _indexCount = count; }
	        std::vector<Vertex>& getVertices() { return _vertices; }
	        std::vector<uint32_t>& getIndices() { return _indices; }
	        std::shared_ptr<render::Buffer>& getVertexBuffer() { return _vertexBuffer; }
	        std::shared_ptr<render::Buffer>& getIndexBuffer() { return _indexBuffer; }
		    glm::vec4 getCollisionSphere() const { return _occlusionSphere; }


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
		    /** Sphere visual collision sphere */
		    glm::vec4 _occlusionSphere {0.0f, 0.0f, 0.0f, 0.0f};
    };
}
