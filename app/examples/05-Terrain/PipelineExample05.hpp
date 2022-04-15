#pragma once

#include "../../../src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp"
#include "../../../src/WaterDropEngine/WdeGUI/WdeGUI.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class PipelineExample05 : public WdeRenderPipelineInstance {
		public:
			void setup() override {
				// Create passes attachments
				setAttachments({
					{0, "Depth texture", RenderAttachment::DEPTH},
					{1, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
				});

				// Create passes and subpasses structure
				setStructure({
					{0, {
						{0, {}, { 0, 1 }},
						{1, {}, { 1 }}
					}}
				});

				// Initialize GUI
				gui::WdeGUI::initialize(std::pair<int, int>{0, 1});
			}

			void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override {
				beginRenderPass(0);
					beginRenderSubPass(0);
						// Draw mesh sync
						for (auto& go : scene.getGameObjects()) {
							// If no mesh or material, continue
							auto mesh = go->getModule<scene::MeshRendererModule>();
							if (!go->active || mesh == nullptr || mesh->getMesh() == nullptr || mesh->getMaterial() == nullptr)
								continue;

							// Bind sets
							bind(commandBuffer, mesh->getMaterial()); // global
							mesh->getMaterial()->bind(commandBuffer); // material
							mesh->getMesh()->bind(commandBuffer); // object

							// Draw
							mesh->getMesh()->render(go->getID());
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
