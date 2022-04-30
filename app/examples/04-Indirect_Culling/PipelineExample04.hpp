#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"
#include "../../../src/WaterDropEngine/WdeRender/pipelines/PipelineCompute.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gizmo/GizmoManager.hpp"
#include "../../../src/WaterDropEngine/WdeScene/culling/CullingInstance.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample04 : public WdeRenderPipelineInstance {
		public:
			std::unique_ptr<wde::scene::CullingInstance> _cullingManager;

			void setup() override {
				// Create passes attachments
				setAttachments({
					{0, "Depth texture", RenderAttachment::DEPTH},
					{1, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
				});

				// Create passes and subpasses structure
				setStructure({
					{0, {
						{0, {}, { 0, 1 }}, // Scene
						{1, {}, { 0, 1 }}, // Gizmo
						{2, {}, { 1 }} // GUI
					}}
				});

				// Create culling manager
				_cullingManager = std::make_unique<wde::scene::CullingInstance>(std::pair<int, int>{0, 0}, _objectsData);

				// Initialize Gizmo
				scene::GizmoManager::initialize(std::pair<int, int>{0, 1});

				// Initialize GUI
				gui::WdeGUI::initialize(std::pair<int, int>{0, 2});
			}

			void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override {
				// Do culling
				/*_cullingManager->createBatches(scene.getGameObjects());

				if (scene.getActiveCamera() != nullptr && scene.getActiveCamera()->name == "Editor Camera")
					_cullingManager->cull(scene.getFirstGameCamera());
				else
					_cullingManager->cull(scene.getActiveCamera());*/

				// Update


				beginRenderPass(0);
					beginRenderSubPass(0);
						//_cullingManager->render(commandBuffer);

						uint32_t iterator = 0;
						for (auto& chunk : scene.getActiveChunks()) {
							for (auto& go : chunk.second->getGameObjects()) {
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
						if (activeGO != nullptr && scene.getActiveGameObject() != nullptr && scene.getActiveGameObject()->getModule<wde::scene::CameraModule>() == nullptr) {
							scene::GizmoManager::_gizmoInstance->setColor(Color::GREEN);
							scene::GizmoManager::_gizmoInstance->drawCube(activeGO->transform->position, activeGO->transform->rotation, activeGO->transform->scale);
						}
					endRenderSubPass();

					beginRenderSubPass(2);
						// Render GUI
						gui::WdeGUI::render(commandBuffer);
					endRenderSubPass();
				endRenderPass();
			}

			void cleanUp() override { }


			// Helper
			static glm::vec4 normalizePlane(glm::vec4 p) {
				return p / glm::length(glm::vec3(p));
			}
	};
}
