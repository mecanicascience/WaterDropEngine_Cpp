#pragma once

#include "../Resource.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "../../WdeRender/pipelines/PipelineGraphics.hpp"
#include "textures/Texture2D.hpp"
#include "textures/TextureCube.hpp"

namespace wde::resource {
	/**
	 * Describes a scene material
	 */
	class Material : public Resource {
		public:
			explicit Material(const std::string& path);
			~Material();
			void drawGUI() override;

			/**
			 * Binds the material to a given command buffer
			 * @param commandBuffer
			 */
			void bind(render::CommandBuffer& commandBuffer);


			// Getters and setters
			std::string getName() const override { return _name; }
			render::PipelineGraphics& getPipeline() { return *_pipeline; }
			uint32_t getID() const { return _materialID; }
			std::pair<int, int> getRenderStage() const { return _renderStage; }


		private:
			/** Material UUID */
			uint32_t _materialID {};
			std::string _name;
			std::unique_ptr<render::PipelineGraphics> _pipeline = nullptr;
			VkPolygonMode _polygonMode {};
			std::string _textureURL;

			/** Material descriptor set */
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _materialSet {};
			/** Material rendering set pass and subpass */
			std::pair<int, int> _renderStage {};
	};
}
