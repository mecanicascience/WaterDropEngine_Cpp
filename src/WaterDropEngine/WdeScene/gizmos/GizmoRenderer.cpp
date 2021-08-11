#include "GizmoRenderer.hpp"
#include "../WdeSceneManager.hpp"

namespace wde::scene {
	GizmoRenderer::GizmoRenderer(RenderStage stage)
		: _pipelineSeparatedLines(
			stage, {"res/shaders/gizmoShader.vert.spv", "res/shaders/gizmoShader.frag.spv"},
			{ scene::Model::Vertex::getDescriptions() },
			PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_LINE,
			VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE
			) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Initializing Gizmo renderer.", LoggerChannel::SCENE);

		// Initialize pipelines
		//_pipelineSeparatedLines.initialize();
	}


	void GizmoRenderer::renderGizmoInstance(CommandBuffer &commandBuffer, Gizmo &gizmoInstance) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Sending Gizmo objects to the GPU and rendering them.", LoggerChannel::SCENE);

		// Render lines
		/*_pipelineSeparatedLines.bind(commandBuffer);
		auto& cameraModule = WdeSceneManager::get().getActiveScene()->getCamera().getModule<scene::CameraModule>();
		PushConstantData push {};
		push.transformWorldSpace  = getTransform({0, 0, 0});
		push.transformCameraSpace = cameraModule.getView();
		push.transformProjSpace   = cameraModule.getProjection();
		_pipelineSeparatedLines.setPushConstants(0, &push);*/
	}



	// Helper functions
	glm::mat4 GizmoRenderer::getTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) const {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
				},
				{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
				},
				{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
				},
				{position.x, position.y, position.z, 1.0f}
		};
	}
}
