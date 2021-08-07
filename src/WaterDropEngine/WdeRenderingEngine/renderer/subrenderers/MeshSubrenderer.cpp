#include "MeshSubrenderer.hpp"

namespace wde::renderEngine {
	/** Matrix used in the shaders to perform projection onto the culling Vulkan space */
	struct PushConstantData {
		glm::mat4 transformWorldSpace {1.0f};
		glm::mat4 transformCameraSpace {1.0f};
		glm::mat4 transformProjSpace {1.0f};
	};


	// Constructors
	MeshSubrenderer::MeshSubrenderer(const RenderStage &stage)
		: Subrenderer(stage),
		  _pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"}, { scene::Model::Vertex::getDescriptions() },
				  PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
				  VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE) {
			// TODO Remove pipeline
			_pipeline.addPushConstants(0, sizeof(PushConstantData));
			_pipeline.initialize();
		}

	// Core functions
	void MeshSubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Rendering scene.", LoggerChannel::RENDERING_ENGINE);

		// Binds the graphics pipeline and model to the command buffer
		// TODO remove pipeline
		_pipeline.bind(commandBuffer);

		// Renders game objects in scene
		if(scene::WdeSceneManager::get().getActiveScene() != nullptr && scene::WdeSceneManager::get().getActiveScene()->hasCamera()) {
			Logger::debug("Rendering game objects.", LoggerChannel::RENDERING_ENGINE);
			scene::Scene& scene = *scene::WdeSceneManager::get().getActiveScene();

			for (auto& go : scene.getGameObjects()) {
				// Get camera
				auto& cameraModule = scene::WdeSceneManager::get().getActiveScene()->getCamera().getModule<scene::CameraModule>();

				// Setup push constants
				PushConstantData push {};
				push.transformWorldSpace  = go->getModule<scene::TransformModule>().getTransform();
				push.transformCameraSpace = cameraModule.getView();
				push.transformProjSpace   = cameraModule.getProjection();
				// Todo remove pipeline
				_pipeline.setPushConstants(0, &push);

				// Render game object
				go->render(commandBuffer);
			}
		}
	}
}
