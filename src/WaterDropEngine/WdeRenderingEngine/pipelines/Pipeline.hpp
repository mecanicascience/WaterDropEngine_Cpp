#pragma once

#include "../../../wde.hpp"
#include "../commands/CommandBuffer.hpp"

namespace wde::renderEngine {
	/**
	 * Class that represents a pipeline
	 */
	class Pipeline : NonCopyable {
		public:
            explicit Pipeline() = default;
			virtual ~Pipeline() = default;

			/**
			 * Binds the specified command buffer to the pipeline
			 * @param commandBuffer
			 */
			void bind(const CommandBuffer &commandBuffer) const {
				vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline());
			}

			virtual const VkPipeline &getPipeline() const = 0;
			virtual const VkPipelineBindPoint &getPipelineBindPoint() const = 0;
	};
}
