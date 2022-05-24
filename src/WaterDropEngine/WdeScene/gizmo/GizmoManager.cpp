#include "GizmoManager.hpp"
#include "../../WaterDropEngine.hpp"

#ifdef WDE_ENGINE_MODE_DEBUG
namespace wde::scene {
	std::unique_ptr<Gizmo> GizmoManager::_gizmoInstance;

	void GizmoManager::initialize(std::pair<int, int> renderStage) {
		WDE_PROFILE_FUNCTION();

		// Create gizmo instance
		GizmoManager::_gizmoInstance = std::make_unique<Gizmo>();

		// Load meshes
		auto& inst = GizmoManager::_gizmoInstance;
		inst->_renderStage = renderStage;

		// Create gizmo descriptors
		inst->_positionsSetBuffer = std::make_shared<render::Buffer>(sizeof(Gizmo::GPUGizmoObjectDescriptor), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		render::DescriptorBuilder::begin()
					.bind_buffer(0, *inst->_positionsSetBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(inst->_positionsSet.first, inst->_positionsSet.second);
		inst->_positionsLinesSetBuffer = std::make_shared<render::Buffer>(sizeof(Gizmo::GPUGizmoLineCamera), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		inst->_positionsLinesSetBufferVertices = std::make_shared<render::Buffer>(sizeof(resource::Vertex) * Config::MAX_GIZMO_OBJECTS_COUNT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		inst->_linesManagerColors = std::make_unique<render::Buffer>(sizeof(Gizmo::GPUGizmoColorDescriptor) * Config::MAX_GIZMO_OBJECTS_COUNT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		render::DescriptorBuilder::begin()
					.bind_buffer(0, *inst->_positionsLinesSetBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.bind_buffer(1, *inst->_linesManagerColors, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(inst->_positionsLinesSet.first, inst->_positionsLinesSet.second);

		// Create gizmo lines pipeline
		auto path = WaterDropEngine::get().getInstance().getScene()->getPath();
		inst->_linesPipeline = std::make_shared<render::PipelineGraphics>(
				renderStage,
                std::vector<std::string>{path + "data/shaders/engine/gizmo/gizmoLines.vert", path + "data/shaders/engine/gizmo/gizmoLines.frag"},
                std::vector<resource::VertexInput>{ resource::Vertex::getDescriptions() },
                render::PipelineGraphics::Mode::Polygon, render::PipelineGraphics::Depth::ReadWrite,
                VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		inst->_linesPipeline->addDescriptorSet(inst->_positionsLinesSet.second);
		inst->_linesPipeline->initialize();

		// Create gizmo meshes
		inst->_meshes.emplace("CUBE", WaterDropEngine::get().getResourceManager().load<resource::Mesh>(
				WaterDropEngine::get().getInstance().getScene()->getPath() + "data/meshes/" + "cube.json"));
	}

	void GizmoManager::render(render::CommandBuffer& commandBuffer) {
		WDE_PROFILE_FUNCTION();

		if (GizmoManager::_gizmoInstance == nullptr)
			throw WdeException(LogChannel::SCENE, "Trying to draw the Gizmo objects without initializing the renderer.");
		auto& inst = GizmoManager::_gizmoInstance;

		// Clear last lines instance
		inst->resetLinesManager();

		// Set command buffer
		inst->_commandBuffer = &commandBuffer;

		// Render Gizmo for instance
		WaterDropEngine::get().getInstance().getPipeline().drawGizmo(*GizmoManager::_gizmoInstance);
		WaterDropEngine::get().getInstance().getScene()->drawGizmo(*GizmoManager::_gizmoInstance);

		// Render Gizmo for modules
		auto scene = WaterDropEngine::get().getInstance().getScene();
		for (auto& ch : scene->getActiveChunks())
			for (auto& go : ch.second->getDynamicGameObjects())
				for (auto& mod : go->getModules())
					mod->drawGizmo(*GizmoManager::_gizmoInstance);

		// Draw Gizmo lines
		inst->drawLinesInstance(commandBuffer);
	}

	void GizmoManager::cleanUp() {
		WDE_PROFILE_FUNCTION();

		if (_gizmoInstance != nullptr) {
			// Clear meshes
			_gizmoInstance->_meshes.clear();

			// Clear instance
			_gizmoInstance.reset();
		}
	}
}
#endif

#ifdef WDE_ENGINE_MODE_PRODUCTION
namespace wde::scene {
	std::unique_ptr<Gizmo> GizmoManager::_gizmoInstance;

	void GizmoManager::initialize(std::pair<int, int> renderStage) {
		GizmoManager::_gizmoInstance = std::make_unique<Gizmo>();
	}
}
#endif
