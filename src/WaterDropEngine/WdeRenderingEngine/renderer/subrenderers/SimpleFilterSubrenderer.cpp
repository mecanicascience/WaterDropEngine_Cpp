#include "SimpleFilterSubrenderer.hpp"

namespace wde::renderEngine {
	// Constructors
	SimpleFilterSubrenderer::SimpleFilterSubrenderer(const RenderStage &stage) : Subrenderer(stage) {}

	// Core functions
	void SimpleFilterSubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Rendering scene game objects at stage " + std::to_string(_stage.first)
		+ "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

		// Renders game objects in scene if the scene has a camera
		//
	}
}
