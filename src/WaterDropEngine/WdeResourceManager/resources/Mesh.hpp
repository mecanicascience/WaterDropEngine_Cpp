#pragma once

#include "../Resource.hpp"
#include "../../WdeRender/commands/CommandBuffer.hpp"
#include "../../WdeRender/buffers/Buffer.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/matrix_transform.hpp>

namespace wde::resource {
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
		alignas(16) glm::vec3 position;
		alignas(16) glm::vec3 normal;
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
					{2, baseBinding, VK_FORMAT_R32G32_SFLOAT   , offsetof(Vertex, uv)}    // UV texture coords (index 2)
			};
			return { bindingDescriptions, attributeDescriptions };
		}
	};



	/**
	 * Describes a scene mesh
	 */
	class Mesh : public Resource {
		public:
			explicit Mesh(const std::string& path);
			~Mesh() override;

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

			// Getters and setters
			std::string getName() const { return _name; }
			int getIndexCount() const { return static_cast<int>(_indexCount); }
			void setIndexCount(uint32_t count) { _indexCount = count; }
			glm::vec4 getCollisionSphere() const { return _occlusionSphere; }


		protected:
			// Core
			/** Name of the mesh */
			std::string _name;
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

namespace std {
	/**
	 * Create a hash function for a vertex instance
	 */
	template<> struct hash<wde::resource::Vertex> {
		size_t operator()(wde::resource::Vertex const& vertex) const {
			return    ((hash<glm::vec3>()(vertex.position)
			            ^ (hash<glm::vec2>()(vertex.uv) << 1)) >> 1)
			          ^ (hash<glm::vec3>()(vertex.normal) << 1);
		}
	};
}
