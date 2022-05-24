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
				//_cullingManager = std::make_unique<wde::scene::CullingInstance>(std::pair<int, int>{0, 0}, _objectsData);

				// Initialize Gizmo
				scene::GizmoManager::initialize(std::pair<int, int>{0, 1});

				// Initialize GUI
				gui::WdeGUI::initialize(std::pair<int, int>{0, 2});
			}

			void drawGizmo(scene::Gizmo& gizmo, render::CommandBuffer& commandBuffer) override;

			void render(CommandBuffer& commandBuffer, scene::WdeSceneInstance &scene) override;

			void cleanUp() override { }


			// Helper
			static glm::vec4 normalizePlane(glm::vec4 p) {
				return p / glm::length(glm::vec3(p));
			}
	};
}
