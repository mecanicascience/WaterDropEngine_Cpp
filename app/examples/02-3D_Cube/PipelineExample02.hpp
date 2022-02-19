#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"
#include "../../../src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample02 : public WdeRenderPipelineInstance {
		public:
			void setup() override {
				// Create passes attachments
				setAttachments({
					{0, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
				});

				// Create passes and subpasses structure
				setStructure({
					{0, {
						{0, {}, { 0 }},
						{1, {}, { 0 }}
					}}
				});

				// Initialize GUI
				gui::WdeGUI::initialize(std::pair<int, int>{0, 1});
			}

			void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance& scene) override {
				beginRenderPass(0);
					beginRenderSubPass(0);
						std::string lastMeshName;

						// Fetch every scene materials
						for (auto& mat : scene.getMaterials()) {
							// Check if material in correct render pass
							if (mat->getRenderStage() != std::pair<int, int>(0, 0))
								continue;

							// Bind global set
							bind(commandBuffer, mat);

							// Bind material
							mat->bind(commandBuffer);

							// Fetch every game object with this material
							for (int goID : mat->getGameObjectsIds()) {
								// Check if GO has a mesh
								auto& go = scene.getGameObject(goID);
								auto& mesh = go.getModule<scene::MeshRendererModule>()->getMesh();
								if (mesh != nullptr) {
									// Bind mesh if different from last one
									if (mesh->getName() != lastMeshName) {
										mesh->bind(commandBuffer);
										lastMeshName = mesh->getName();
									}

									// Render mesh
									mesh->render(go.getID());
								}
							}
						}
					endRenderSubPass();

					beginRenderSubPass(1);
						// Render GUI
						gui::WdeGUI::render(commandBuffer);
					endRenderSubPass();
				endRenderPass();
			}

			void cleanUp() override { }
	};
}
