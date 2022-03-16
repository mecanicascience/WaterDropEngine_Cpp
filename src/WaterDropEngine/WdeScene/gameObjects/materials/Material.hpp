#pragma once

#include "../../../../wde.hpp"
#include "../meshes/Vertex.hpp"
#include "../../../WdeRender/pipelines/PipelineGraphics.hpp"
#include "../../../WdeRender/buffers/Buffer.hpp"

namespace wde::scene {
	class Material : public NonCopyable {
		public:
			/**
			 * Create a new material
			 * @param name
			 * @param renderStage
			 * @param shaders
			 * @param polygonMode (default : mode fill)
			 */
			explicit Material(std::string name, std::pair<int, int> renderStage, const std::vector<std::string>& shaders, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL);
            ~Material() override;


			// Core functions
			/**
			 * Create the material
			 */
			void createMaterial();

			/**
			 * Binds the material to the command buffer
			 * @param commandBuffer
			 */
			void bind(render::CommandBuffer& commandBuffer);


			// Getters and setters
			std::string getName() const { return _name; }
			std::pair<int, int> getRenderStage() const { return _renderStage; }
			render::PipelineGraphics& getPipeline() { return *_pipeline; }
			int getID() const { return _materialID; }
			void addDescriptorSet(std::pair<VkDescriptorSet, VkDescriptorSetLayout>& set) { _descriptorsSetsOther.push_back(set); }


		protected:
			/** Material descriptor set */
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _materialSet;
			std::vector<std::pair<VkDescriptorSet, VkDescriptorSetLayout>> _descriptorsSetsOther {};

			// Core data
			/** Material unique ID */
			int _materialID;
			/** Material name */
			std::string _name;
			/** Render pass and subpass of the material */
			std::pair<int, int> _renderStage;
			/** Material pipeline */
			std::unique_ptr<render::PipelineGraphics> _pipeline;
	};
}
