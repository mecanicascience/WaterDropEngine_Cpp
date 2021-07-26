#pragma once

#include "descriptors/RenderStage.hpp"
#include "../commands/CommandBuffer.hpp"

namespace wde::renderEngine {
	/**
	 * Represents a render stage that is used to render a type of pipeline
	 */
	class Subrenderer {
		public:
			// Core functions
			/**
			 * Creates a new renderer stage used to render a type of pipeline
			 * @param stage The stage this renderer will be used in
			 */
            explicit Subrenderer(const RenderStage &stage) : _stage(stage) {}
			virtual ~Subrenderer() = default;

			/**
			 * Renders the sub-renderer on the provided command buffer
			 * @param commandBuffer
			 */
			virtual void render(const CommandBuffer &commandBuffer) = 0;


			// Getters and setters
			bool isEnabled() const { return _enabled; }
			void setEnabled(bool enable) { this->_enabled = enable; }



		private:
			/** The render stage of this sub-renderer */
			const RenderStage& _stage;

			/** The render stage enabled status (activated by default) */
			bool _enabled = true;
	};
}
