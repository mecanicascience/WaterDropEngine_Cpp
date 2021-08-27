#include "MeshSubrenderer.hpp"

namespace wde::renderEngine {
	// Constructors
	MeshSubrenderer::MeshSubrenderer(const RenderStage &stage) : Subrenderer(stage) {}

	// Core functions
	void MeshSubrenderer::preRender(CommandBuffer &commandBuffer) {
        WDE_PROFILE_FUNCTION();
        Logger::debug("Pre-rendering scene game objects at stage " + std::to_string(_stage.first)
                      + "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

        // Renders game objects in scene if the scene has a camera
        if(scene::WdeSceneManager::get().hasScene())
            scene::WdeSceneManager::get().getActiveScene()->preRender(commandBuffer, _stage);
	}

	void MeshSubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Rendering scene game objects at stage " + std::to_string(_stage.first)
						+ "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

		// Renders game objects in scene if the scene has a camera
		if(scene::WdeSceneManager::get().hasScene() && scene::WdeSceneManager::get().getActiveScene()->hasCamera())
			scene::WdeSceneManager::get().getActiveScene()->render(commandBuffer, _stage);
	}
}
