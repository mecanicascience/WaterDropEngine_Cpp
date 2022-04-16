#include "WdeInstance.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde {
	WdeInstance::WdeInstance() {
		// Create the engine instance
		WaterDropEngine::get();
	}

	void WdeInstance::startInstance() {
		// Start the engine
		WaterDropEngine::get().start(*this);
	}

	void WdeInstance::tickInstance() {
		WDE_PROFILE_FUNCTION();
		// Check if there is scene and pipeline
		if (_scene == nullptr)
			throw WdeException(LogChannel::CORE, "The engine has no scene.");
		if (_pipeline == nullptr)
			throw WdeException(LogChannel::CORE, "The engine has no render pipeline.");

		// Update the engine
		update();

		// Tick for the engine pipeline
		_pipeline->tick();

		// Tick for the scene
		_scene->tick();
	}

	void WdeInstance::cleanUpInstance() {
		// Destroy render scene
		if (_scene != nullptr) {
			_scene->cleanUpInstance();
			_scene.reset();
		}

		// Destroy render pipeline
		_pipeline->cleanUp();
		_pipeline.reset();

		// Clean up instance
		cleanUp();
	}





	void WdeInstance::setRenderPipeline(std::shared_ptr<render::WdeRenderPipelineInstance> pipeline) {
		WDE_PROFILE_FUNCTION();
		_pipeline = std::move(pipeline);
		_pipeline->setup();
	}

	void WdeInstance::setScene(std::shared_ptr<scene::WdeSceneInstance> scene) {
		WDE_PROFILE_FUNCTION();
		_scene = std::move(scene);

		// Add editor camera
#ifdef WDE_ENGINE_MODE_DEBUG
		{
			auto camera = _scene->createGameObject("Editor Camera");
			auto camModule = camera->addModule<scene::CameraModule>();
			camModule->setAsActive();
			camModule->setFarPlane(std::numeric_limits<float>::max());
			_scene->setActiveCamera(camera.get());
			camera->addModule<scene::ControllerModule>();
			camera->transform->position = glm::vec3 {0.0f, 0.0f, 0.0f};
			camera->transform->rotation = glm::vec3 {0.0f, 0.0f, 0.0f};
		}
#endif

		// Setup scene
		_scene->setup();
	}
}
