#include "PipelineExample04.hpp"
#include "../../../src/WaterDropEngine/WaterDropEngine.hpp"

namespace examples {
	void PipelineExample04::render(CommandBuffer &commandBuffer, scene::WdeSceneInstance &scene) {
		// Do culling
		/*_cullingManager->createBatches(scene.getGameObjects());

		if (scene.getActiveCamera() != nullptr && scene.getActiveCamera()->name == "Editor Camera")
			_cullingManager->cull(scene.getFirstGameCamera());
		else
			_cullingManager->cull(scene.getActiveCamera());*/

		beginRenderPass(0);
			beginRenderSubPass(0);
				//_cullingManager->render(commandBuffer);

				uint32_t iterator = 0;
				for (auto &chunk: scene.getActiveChunks()) {
					for (auto &go: chunk.second->getGameObjects()) {
						iterator++;

						// If no mesh or material, continue
						auto mesh = go->getModule<scene::MeshRendererModule>();
						if (!go->active || mesh == nullptr || mesh->getMesh() == nullptr || mesh->getMaterial() == nullptr)
							continue;

						// Bind sets
						bind(commandBuffer, mesh->getMaterial()); // global
						mesh->getMaterial()->bind(commandBuffer); // material
						mesh->getMesh()->bind(commandBuffer); // object

						// Draw
						mesh->getMesh()->render(iterator - 1);
					}
				}
			endRenderSubPass();

			beginRenderSubPass(1);
				// Render Gizmo
				scene::GizmoManager::render(commandBuffer);

				// Draw gizmo on active game object
				auto activeGO = scene.getActiveGameObject();
				if (activeGO != nullptr && scene.getActiveGameObject() != nullptr &&
				    scene.getActiveGameObject()->getModule<wde::scene::CameraModule>() == nullptr) {
					scene::GizmoManager::_gizmoInstance->setColor(Color::GREEN);
					scene::GizmoManager::_gizmoInstance->drawCube(activeGO->transform->position, activeGO->transform->rotation,
					                                              activeGO->transform->scale);
				}
			endRenderSubPass();

			beginRenderSubPass(2);
				// Render GUI
				gui::WdeGUI::render(commandBuffer);
			endRenderSubPass();
		endRenderPass();
	}

	void PipelineExample04::drawGizmo(scene::Gizmo &gizmo, CommandBuffer &commandBuffer) {

	}
}
