#pragma once

#include "../commands/CommandBuffer.hpp"
#include "passes/RenderAttachment.hpp"
#include "passes/RenderPass.hpp"
#include "passes/RenderPassStructure.hpp"

namespace wde::render {
	/**
	 * Represents an instance of the engine rendering pipeline
	 */
	class WdeRenderPipeline {
		public:
			WdeRenderPipeline() = default;
			~WdeRenderPipeline() {
				// Destroy render passes
				_passes.clear();
			}

			// Core WDE functions
			/** Tick for pipeline (called by the WdeInstance) */
			void tick();
			/**
			 * Called when the window is resized (called by the WdeInstance)
			 */
			void onWindowResized() {
				// Recreate render passes
				_passes.clear();
				setStructure(_structure);
			}


			/**
			 * Create the render passes and subpasses (called by the WdeInstance)
			 */
			virtual void setup() = 0;

			/**
			 * Clean up the pipeline resources
			 */
			virtual void cleanUp() = 0;


			// Getters and setters
			RenderPass& getRenderPass(uint32_t renderPassID) { return *_passes[renderPassID]; }



		protected:
			// Inherited rendering methods
			/**
			 * Render the pipeline to a command buffer
			 * @param commandBuffer
			 */
			virtual void render(CommandBuffer& commandBuffer) = 0;



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
