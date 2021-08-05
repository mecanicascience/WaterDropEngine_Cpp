#include "CoreAppSubrenderer.hpp"
#include "../../src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.hpp"


struct PushConstantData {
	glm::mat4 transformWorldSpace {1.0f};
	glm::mat4 transformCameraSpace {1.0f};
	glm::mat4 transformProjSpace {1.0f};
};


CoreAppSubrenderer::CoreAppSubrenderer(const RenderStage &stage)
		: Subrenderer(stage),
		  _pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"}, { Model::Vertex::getDescriptions() },
					PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
	                VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE) {
	// Set pipeline push constants infos
	_pipeline.addPushConstants(0, sizeof(PushConstantData));

	// Initialize pipeline
	_pipeline.initialize();

	// Create camera viewing object
	_cameraViewerObject = GameObject::createGameObject();
	_cameraViewerObject.transform.translation = {0.0f, 0.0f, 0.0f};
	//_camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f)); // Camera look to the right
	_camera.setViewTarget(_cameraViewerObject.transform.translation, glm::vec3(0.0f, 0.0f, 0.0f));

}

void CoreAppSubrenderer::render(CommandBuffer &commandBuffer) {
	auto newTime = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - _currentTime).count();
	_currentTime = newTime;

	// Update camera projection matrix
	auto aspect = CoreInstance::get().getSelectedDevice().getSwapChain().getAspectRatio();
	//_camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
	_camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

	// Update camera position and rotation
	_cameraController.moveInPlaneXZ(dt, _cameraViewerObject);
	_camera.setViewYXZ(_cameraViewerObject.transform.translation, _cameraViewerObject.transform.rotation);

	// Binds the graphics pipeline and model to the command buffer
	_pipeline.bind(commandBuffer);

	// Render game objects
	for (auto &obj : _gameObjects) {
		// Setup push constants
		PushConstantData push {};
		push.transformWorldSpace  = obj.transform.mat4();
		push.transformCameraSpace = _camera.getView();
		push.transformProjSpace   = _camera.getProjection();
		_pipeline.setPushConstants(0, &push);

		// Render game object
		obj.render(commandBuffer);
	}
}
