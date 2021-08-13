#include "GizmoSubrenderer.hpp"

namespace wde::renderEngine {
	GizmoSubrenderer::GizmoSubrenderer(const RenderStage &stage) : Subrenderer(stage), _gizmoRenderer(stage) {}

	void GizmoSubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Rendering scene Gizmo objects at stage " + std::to_string(_stage.first)
			+ "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

		// Scene game objects can push Gizmo objects to the stack
		Gizmo gizmoInstance {};
		if(WdeSceneManager::get().getActiveScene() != nullptr && WdeSceneManager::get().getActiveScene()->hasCamera())
			WdeSceneManager::get().getActiveScene()->renderGizmo(gizmoInstance);

		// Render Gizmo objects pushed to the stack
		_gizmoRenderer.renderGizmoInstance(commandBuffer, gizmoInstance);
	}
}
