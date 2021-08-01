#include "CoreAppSubrenderer.hpp"
#include "../src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.hpp"


struct PushConstantData {
	glm::mat4 transform {1.0f};
};


CoreAppSubrenderer::CoreAppSubrenderer(const RenderStage &stage)
		: Subrenderer(stage),
		  _pipeline(stage, {"res/shaders/simpleShader.vert.spv", "res/shaders/simpleShader.frag.spv"}, { Model::Vertex::getDescriptions() },
					PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
	                VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE) {
	// Set pipeline push constants infos
	_pipeline.addPushConstants(0, sizeof(PushConstantData));

	// Initialize pipeline
	_pipeline.initialize();

	// Set camera initial viewing direction
	// _camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f)); // Camera look to the right
	_camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 2.5f));

	// Create camera viewing object
	_cameraViewerObject = GameObject::createGameObject();
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
	auto projectionView = _camera.getProjection() * _camera.getView();
	for (auto &obj : _gameObjects) {
		// Setup push constants
		PushConstantData push {};
		push.transform = projectionView * obj.transform.mat4();
		_pipeline.setPushConstants(0, &push);

		// Render game object
		obj.render(commandBuffer);
	}
}
