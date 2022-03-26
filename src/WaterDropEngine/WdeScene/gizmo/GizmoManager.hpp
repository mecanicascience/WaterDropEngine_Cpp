#pragma once

#include "Gizmo.hpp"
#include "../../WdeRender/commands/CommandBuffer.hpp"

namespace wde::scene {
#ifdef WDE_ENGINE_MODE_DEBUG
	/**
	 * Update every gizmo game objects and render them to the screen
	 */
	class GizmoManager : public NonCopyable {
		public:
			/** Create the different gizmo rendering pipelines */
			static void initialize(std::pair<int, int> renderStage);

			/** Update every gizmo game objects and render them to the screen */
			static void render(render::CommandBuffer& commandBuffer);

			/** Clean up instance */
			static void cleanUp();

			// Data
			/** The drawing gizmo instance */
			static std::unique_ptr<Gizmo> _gizmoInstance;
	};
#endif

#ifdef WDE_ENGINE_MODE_PRODUCTION
	class GizmoManager {
		public:
			static void initialize(std::pair<int, int> renderStage);
			static void render(render::CommandBuffer& commandBuffer) {}
			static void cleanUp() {}
			static std::unique_ptr<Gizmo> _gizmoInstance;
	};
#endif
}

