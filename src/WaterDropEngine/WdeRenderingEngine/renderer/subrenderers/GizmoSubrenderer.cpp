#include "GizmoSubrenderer.hpp"

namespace wde::renderEngine {
	GizmoSubrenderer::GizmoSubrenderer(const RenderStage &stage) : Subrenderer(stage) {}

	void GizmoSubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Rendering scene Gizmo objects at stage " + std::to_string(_stage.first)
			+ "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

		// Renders game objects in scene if the scene has a camera
		/*if(scene::WdeSceneManager::get().getActiveScene() != nullptr && scene::WdeSceneManager::get().getActiveScene()->hasCamera()) {
			scene::Scene& scene = *scene::WdeSceneManager::get().getActiveScene();
			for (auto& go : scene.getGameObjects())
				go->renderGizmo(commandBuffer, _stage, scene::WdeSceneManager::get().getActiveScene()->getCamera());
		}*/
	}
}
