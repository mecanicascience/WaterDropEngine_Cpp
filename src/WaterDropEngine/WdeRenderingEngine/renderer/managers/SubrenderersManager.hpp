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
			  * @param stage The Subrenderers renderer stage
			  * @param subrender The subrenderers
			  * @return The added renderer
			  */
			template<typename T, typename... Args>
			T *add(const RenderStage &stage, std::unique_ptr<T> &&subrender) {
				const auto typeID = TypeInfo<Subrenderer>::getTypeID<T>();

				// Insert the stage value to the list
				_stages.insert({StageIndex(stage, _subrenders.size()), typeID});

				// Then, add the sub-renderer to list as <TypeID, Sub-renderer>
				_subrenders[typeID] = std::move(subrender);
				return static_cast<T *>(_subrenders[typeID].get());
			}

			/** Initialize renderers */
			void initializeRenderers() {
				WDE_PROFILE_FUNCTION();

				// Initialize sub-renderers
				for (const auto &[renderID, subrenderer] : _subrenders)
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
				for (const auto &[stageIndex, typeID] : _stages) {
					if (stageIndex.first != stage)
						continue;

					// Renders all the sub-renderers corresponding enabled and corresponding to the stage
					if (auto &subrender = _subrenders[typeID]) {
						if (subrender->isEnabled()) {
							subrender->render(commandBuffer);
						}
					}
				}
			}




		private:
			using StageIndex = std::pair<RenderStage, std::size_t>;

			/** List of all Subrenders */
			std::unordered_map<TypeID, std::unique_ptr<Subrenderer>> _subrenders;
			/** List of subrender stages. */
			std::multimap<StageIndex, TypeID> _stages;
	};
}
