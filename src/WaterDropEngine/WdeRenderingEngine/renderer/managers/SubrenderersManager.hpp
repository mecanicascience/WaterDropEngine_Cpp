#pragma once

#include <map>

#include "../descriptors/RenderStage.hpp"
#include "../managers/TypeInfo.hpp"
#include "../../subrenderers/Subrenderer.hpp"

namespace wde::renderEngine {
	class SubrenderersManager : NonCopyable {
		public:
			/**
			  * Adds a Sub-renderer to the list
			  * @tparam T The Subrenderers type
			  * @param stage The renderer stage of the subrenderer
			  * @param subrender The subrenderer
			  * @return The added renderer
			  */
			template<typename T, typename... Args>
			T *add(const RenderStage &stage, std::unique_ptr<T> &&subrender) {
				// Insert the subrenderer to the list
				_subrenders.push_back(std::move(subrender));
				_subrendererStages[stage].push_back(_subrenders.size() - 1);

				return static_cast<T *>(_subrenders[_subrenders.size() - 1].get());
			}

			/** Initialize renderers */
			void initializeRenderers() {
				WDE_PROFILE_FUNCTION();

				// Initialize sub-renderers
				for (auto& subrenderer : _subrenders)
					if (subrenderer->isEnabled())
						subrenderer->initialize();
			}

			/**
			 * Renders a given stage
			 * @param stage The renderer chain to renderer
			 * @param commandBuffer The corresponding command buffer
			 */
			void renderStage(const RenderStage &stage, CommandBuffer &commandBuffer) {
                WDE_PROFILE_FUNCTION();
                Logger::debug("Rendering sub-renderers for stage " + std::to_string(stage.first) + "-" + std::to_string(stage.second) + ".",
							  LoggerChannel::RENDERING_ENGINE);

                // Render subrenders at given stage
                for (int id : _subrendererStages[stage]) {
                    if (_subrenders[id]->isEnabled())
                    	_subrenders[id]->render(commandBuffer);
                }
			}




		private:
			/** List of each render stages, and their corresponding subrender IDs in the subrenderer array */
			std::map<RenderStage, std::vector<int>> _subrendererStages {};
			/** An array of pointers to the subrenderers */
			std::vector<std::unique_ptr<Subrenderer>> _subrenders {};
	};
}
