#include "MeshSubrenderer.hpp"

namespace wde::renderEngine {
	// Constructors
	MeshSubrenderer::MeshSubrenderer(const RenderStage &stage) : Subrenderer(stage) {}

	// Core functions
	void MeshSubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Rendering scene game objects at stage" + std::to_string(_stage.first)
						+ "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

		// Renders game objects in scene if the scene has a camera
		if(scene::WdeSceneManager::get().getActiveScene() != nullptr && scene::WdeSceneManager::get().getActiveScene()->hasCamera())
			scene::WdeSceneManager::get().getActiveScene()->render(commandBuffer, _stage);
	}
}
