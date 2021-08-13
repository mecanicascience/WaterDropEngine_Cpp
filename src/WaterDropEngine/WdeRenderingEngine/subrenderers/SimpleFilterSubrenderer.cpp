#include "SimpleFilterSubrenderer.hpp"

namespace wde::renderEngine {
	// Constructors
	SimpleFilterSubrenderer::SimpleFilterSubrenderer(const RenderStage &stage)
		: Subrenderer(stage),
			_pipeline(stage, {"res/shaders/simpleFilter/simpleFilter.vert.spv", "res/shaders/simpleFilter/simpleFilter.frag.spv"},
				  { },
				  PipelineGraphics::Mode::MRT, PipelineGraphics::Depth::ReadWrite,
				  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
				  VK_CULL_MODE_NONE) {}

	// Core functions
	void SimpleFilterSubrenderer::initialize() {
		// Initialize descriptor
		auto& frameBuffers = WdeRenderEngine::get().getRenderer()->getRenderPass(_stage.first)->getFrameBuffers();

		_descriptor = std::make_shared<Descriptor>();
		_descriptor->addSet(0, {
			{0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, frameBuffers.getImageAttachment(1)->getView()}
		});
		/*_descriptor->addSet(0, {
			{0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, frameBuffers.getImageAttachment(0)->getView()}
		});*/

		// Setup pipeline
		_pipeline.addDescriptor(_descriptor);
		_pipeline.initialize();
	}

	void SimpleFilterSubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Rendering simple filter at stage " + std::to_string(_stage.first)
			+ "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

		// Bind pipeline
		_pipeline.bind(commandBuffer);
		_pipeline.bind(_descriptor);

		// Renders game objects in scene if the scene has a camera
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	}
}
