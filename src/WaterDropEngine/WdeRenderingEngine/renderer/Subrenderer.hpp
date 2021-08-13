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

            /** Initialize the sub-renderer */
            virtual void initialize() {};
			/**
			 * Renders the sub-renderer on the provided command buffer
			 * @param commandBuffer
			 */
			virtual void render(CommandBuffer &commandBuffer) = 0;


			// Getters and setters
			bool isEnabled() const { return _enabled; }
			void setEnabled(bool enable) { this->_enabled = enable; }



		protected:
			/** The render stage of this sub-renderer */
			RenderStage _stage;

			/** The render stage enabled status (activated by default) */
			bool _enabled = true;
	};
}
