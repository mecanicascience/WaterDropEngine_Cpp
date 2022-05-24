#pragma once

#include "commands/CommandBuffer.hpp"
#include "render/RenderAttachment.hpp"
#include "render/RenderPass.hpp"
#include "render/RenderPassStructure.hpp"
#include "descriptors/DescriptorBuilder.hpp"
#include "buffers/Buffer.hpp"
#include "pipelines/Pipeline.hpp"
#include "../WdeScene/WdeSceneInstance.hpp"
#include "../WdeScene/modules/CameraModule.hpp"
#include "../WdeScene/modules/MeshRendererModule.hpp"

namespace wde::render {
	/**
	 * Represents an instance of the engine rendering pipeline
	 */
	class WdeRenderPipelineInstance : public core::Observer {
		public:
			WdeRenderPipelineInstance() = default;
			~WdeRenderPipelineInstance() override;

			// Core WDE functions
			/** Tick for pipeline (called by the WdeInstance) */
			void tick();
			/** Called when the window is resized (called by the WdeInstance) */
			void onWindowResized();


			// Inherited rendering methods
			/**
			 * Create the render passes and sub-passes (called by the WdeInstance)
			 */
			virtual void setup() = 0;
			/**
			 * Render the pipeline to a command buffer
			 * @param commandBuffer
			 * @param scene
			 */
			virtual void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) = 0;
			/**
			 * On a GUI event or engine event is received
			 * @param event
			 */
			void onNotify(const core::Event& event) override {};
			/** Draw the gizmo elements to the scene */
			virtual void drawGizmo(scene::Gizmo& gizmo, render::CommandBuffer& commandBuffer) {};
			/**
			 * Clean up the pipeline resources
			 */
			virtual void cleanUp() = 0;

			// Getters and setters
			RenderPass& getRenderPass(uint32_t renderPassID) { return *_passes[renderPassID]; }



		protected:
			// Pass command manager
			/** Set the engine rendering attachments */
			 void setAttachments(const std::vector<RenderAttachment>& attachments) {
				_attachments = attachments;
			 }

			/** Set the engine rendering structure */
			void setStructure(const std::vector<RenderPassStructure>& structure);



			// Render passes commands
			void beginRenderPass(uint32_t index);
			void endRenderPass();

			void beginRenderSubPass(uint32_t index);
			void endRenderSubPass();



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
