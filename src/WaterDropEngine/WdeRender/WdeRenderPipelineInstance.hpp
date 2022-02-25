#pragma once

#include "commands/CommandBuffer.hpp"
#include "render/RenderAttachment.hpp"
#include "render/RenderPass.hpp"
#include "render/RenderPassStructure.hpp"
#include "descriptors/DescriptorBuilder.hpp"
#include "buffers/Buffer.hpp"
#include "pipelines/Pipeline.hpp"
#include "../WdeScene/gameObjects/materials/Material.hpp"
#include "../WdeScene/WdeSceneInstance.hpp"
#include "../WdeScene/gameObjects/modules/CameraModule.hpp"
#include "../WdeScene/gameObjects/modules/MeshRendererModule.hpp"

namespace wde::render {
	/**
	 * Represents an instance of the engine rendering pipeline
	 */
	class WdeRenderPipelineInstance {
		public:
			WdeRenderPipelineInstance();
			~WdeRenderPipelineInstance() {
				// Destroy render passes
				_passes.clear();
			}

			// Core WDE functions
			/** Tick for pipeline (called by the WdeInstance) */
			void tick();
			/** Called when the window is resized (called by the WdeInstance) */
			void onWindowResized() {
				WDE_PROFILE_FUNCTION();
				// Recreate render passes
				_passes.clear();
				setStructure(_structure);
			}


			/**
			 * Create the render passes and sub-passes (called by the WdeInstance)
			 */
			virtual void setup() = 0;

			/**
			 * Clean up the pipeline resources
			 */
			virtual void cleanUp() = 0;


			// Draw commands
			/**
			 * Bind the global set to a command buffer
			 * @param commandBuffer
			 * @param material
			 */
			void bind(CommandBuffer& commandBuffer, scene::Material* material) {
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				                        material->getPipeline().getLayout(), 0, 1, &_globalSet.first, 0, nullptr);
			}


			// Getters and setters
			RenderPass& getRenderPass(uint32_t renderPassID) { return *_passes[renderPassID]; }
			std::pair<VkDescriptorSet, VkDescriptorSetLayout>& getGlobalSet() { return _globalSet; }
			render::Buffer& getCameraBuffer() { return *_cameraData; }



		protected:
			// Inherited rendering methods
			/**
			 * Render the pipeline to a command buffer
			 * @param commandBuffer
			 * @param scene
			 */
			virtual void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) = 0;



			// Pass command manager
			/** Set the engine rendering attachments */
			 void setAttachments(const std::vector<RenderAttachment>& attachments) {
				_attachments = attachments;
			 }

			/** Set the engine rendering structure */
			void setStructure(const std::vector<RenderPassStructure>& structure) {
				WDE_PROFILE_FUNCTION();
				_structure = structure;

				// Check if attachments setup
				if (_attachments.empty())
					throw WdeException(LogChannel::RENDER, "Tried to create render passes before creating attachments in the render pipeline.");

				// Create passes
				uint32_t iterator = 0;
				for (auto& str : structure) {
					// Check if passes IDs are in order
					if (iterator != str._passID)
						throw WdeException(LogChannel::RENDER, "Missing render pass with ID = " + std::to_string(iterator) + ".");

					// Check if subpasses IDs are in order
					uint32_t iterator2 = 0;
					for (auto& sub : str._subPasses) {
						if (iterator2 != sub._subpassID)
							throw WdeException(LogChannel::RENDER,
							                   "Missing render subpass with ID = " + std::to_string(iterator2) +
							                   " in render pass with ID = " + std::to_string(iterator) + ".");
						iterator2++;
					}

					// Create subpass
					_passes.push_back(std::make_unique<RenderPass>(_attachments, str._subPasses));
					iterator++;
				}
			}



			// Render passes commands
			void beginRenderPass(uint32_t index) {
				WDE_PROFILE_FUNCTION();
				if (_currentRenderPassID != -1)
					throw WdeException(LogChannel::RENDER, "Trying to begin pass " + std::to_string(index) + " while pass " + std::to_string(_currentRenderPassID) + " has already began.");
				if (index >= _passes.size())
					throw WdeException(LogChannel::RENDER, "Trying to begin pass " + std::to_string(index) + " which wasn't created.");

				_currentRenderPassID = index;
				_passes[_currentRenderPassID]->start();
			}

			void endRenderPass() {
				WDE_PROFILE_FUNCTION();
				if (_currentRenderSubPassID != -1)
					throw WdeException(LogChannel::RENDER, "Trying to end render pass " + std::to_string(_currentRenderPassID) + " while subpass " + std::to_string(_currentRenderSubPassID) + " has already began.");

				_passes[_currentRenderPassID]->end();
				_currentRenderPassID = -1;
			}



			void beginRenderSubPass(uint32_t index) {
				WDE_PROFILE_FUNCTION();
				if (_currentRenderPassID == -1)
					throw WdeException(LogChannel::RENDER, "Trying to begin subpass " + std::to_string(index) + " outside of a render pass.");
				if (_currentRenderSubPassID != -1)
					throw WdeException(LogChannel::RENDER, "Trying to begin subpass " + std::to_string(index) + " while subpass " + std::to_string(_currentRenderSubPassID)
						+ " has already began in render pass " + std::to_string(_currentRenderPassID));
				if (index >= _passes[_currentRenderPassID]->getSubPassesCount())
					throw WdeException(LogChannel::RENDER, "Trying to begin pass " + std::to_string(index) + " which wasn't created.");

				_currentRenderSubPassID = index;
				_passes[_currentRenderPassID]->startSubPass(index);
			}

			void endRenderSubPass() {
				WDE_PROFILE_FUNCTION();
				if (_currentRenderPassID == -1)
					throw WdeException(LogChannel::RENDER, "Trying to end subpass " + std::to_string(_currentRenderSubPassID) + " outside of a render pass.");

				_passes[_currentRenderPassID]->endSubPass(_currentRenderSubPassID);
				_currentRenderSubPassID = -1;
			}




			// ======= SETS DATA =========
			// Passes common descriptor sets
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _globalSet;

			// Global set
			/** Camera data */
			struct GPUCameraData {
				glm::mat4 view {1.0f};
				glm::mat4 proj {1.0f};
			};
			std::unique_ptr<render::Buffer> _cameraData;

			/** Game objects data */
			std::unique_ptr<render::Buffer> _objectsData;



		private:
			// Passes description
			std::vector<RenderAttachment> _attachments;
			std::vector<RenderPassStructure> _structure;
			std::vector<std::unique_ptr<RenderPass>> _passes;

			// Passes render storage
			uint32_t _currentRenderPassID = -1; // no pass = -1
			uint32_t _currentRenderSubPassID = -1; // no pass = -1
	};
}
