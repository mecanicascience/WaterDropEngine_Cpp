#pragma once

#include "../../../../wde.hpp"
#include "../meshes/Vertex.hpp"
#include "../../../WdeRender/pipelines/PipelineGraphics.hpp"
#include "../../../WdeRender/buffers/Buffer.hpp"

namespace wde::scene {
	class Material {
		public:
			explicit Material(std::string  name, std::pair<int, int> renderStage, const std::vector<std::string>& shaders);
            ~Material();


			// Core functions
			/**
			 * Create the material
			 */
			void createMaterial();

			/**
			 * Binds the material to the command buffer
			 * @param commandBuffer
			 */
			void bind(render::CommandBuffer& commandBuffer) {
				WDE_PROFILE_FUNCTION();

				// Bind material descriptor
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
										_pipeline->getLayout(), 1, 1, &_materialSet.first, 0, nullptr);

				// Bind pipeline
                _pipeline->bind(commandBuffer);
			}


			// GameObject manager
			/**
			 * Assign a new game object to the material
			 * @param goID GameObject ID
			 */
			void addGameObject(int goID) {
				_boundedGoIds.push_back(goID);
			}

			/**
			 * Remove a game object to the material
			 * @param goID GameObject ID
			 */
			void removeGameObject(int goID) {
				_boundedGoIds.erase(std::remove(_boundedGoIds.begin(), _boundedGoIds.end(), goID), _boundedGoIds.end());
			}

			/** @return the list of every game object ids with this material */
			std::vector<int> getGameObjectsIds() { return _boundedGoIds; }


			// Getters and setters
			std::string getName() { return _name; }
			std::pair<int, int> getRenderStage() { return _renderStage; }
			render::PipelineGraphics& getPipeline() { return *_pipeline; }


		protected:
			/** IDs of the game objects with materials corresponding to this material */
			std::vector<int> _boundedGoIds {};
			/** Material descriptor set */
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _materialSet;

			// Core data
			/** Material name */
			std::string _name;
			/** Render pass and subpass of the material */
			std::pair<int, int> _renderStage;
			/** Material pipeline */
			std::unique_ptr<render::PipelineGraphics> _pipeline;
	};
}
