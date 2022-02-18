#pragma once

#include "../../../../wde.hpp"
#include "../meshes/Vertex.hpp"
#include "../../../WdeRender/pipelines/PipelineGraphics.hpp"

namespace wde::scene {
	class Material {
		public:
			explicit Material(const std::string& name, std::pair<int, int> renderStage, std::vector<std::string> shaders)
			    : _name(name),
			      _renderStage(renderStage),
			      _pipeline(std::make_unique<render::PipelineGraphics>(
                            renderStage,
                            shaders, // Shaders
                            std::vector<VertexInput>{ Vertex::getDescriptions() }, // Vertices
                            render::PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
                            render::PipelineGraphics::Depth::None,    // Do not use depth
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
                            VK_POLYGON_MODE_FILL,   // Fill drawn shapes
                            VK_CULL_MODE_NONE)) {  // Disable pipeline culling
			    // Add material descriptors
                //_pipeline->addDescriptorStructure(_desc); // For now, no descriptor

                // Initialize pipeline
                _pipeline->initialize();
            }
            ~Material() {
			    _pipeline.reset();
			}


			// Core functions
			/**
			 * Binds the material to the command buffer
			 * @param commandBuffer
			 */
			void bind(render::CommandBuffer& commandBuffer) {
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


		private:
			/** IDs of the game objects with materials corresponding to this material */
			std::vector<int> _boundedGoIds {};

			// Core data
			/** Material name */
			std::string _name;
			/** Render pass and subpass of the material */
			std::pair<int, int> _renderStage;
			/** Material pipeline */
			std::unique_ptr<render::PipelineGraphics> _pipeline;
	};
}
