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
					.bind_buffer(0, &inst->_positionsSetBuffer->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(inst->_positionsSet.first, inst->_positionsSet.second);
		inst->_positionsLinesSetBuffer = std::make_shared<render::Buffer>(sizeof(Gizmo::GPUGizmoLineDescriptor), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		inst->_positionsLinesSetBufferVertices = std::make_shared<render::Buffer>(sizeof(Vertex) * Config::MAX_GIZMO_OBJECTS_COUNT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		render::DescriptorBuilder::begin()
					.bind_buffer(0, &inst->_positionsLinesSetBuffer->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(inst->_positionsLinesSet.first, inst->_positionsLinesSet.second);

		// Create gizmo meshes
		inst->_meshes.emplace("CUBE", std::make_shared<MeshLoader>("cube.obj"));
	}

	void GizmoManager::render(render::CommandBuffer& commandBuffer) {
		WDE_PROFILE_FUNCTION();
		auto& gameObjects = WaterDropEngine::get().getInstance().getScene().getDynamicGameObjects();

		if (GizmoManager::_gizmoInstance == nullptr)
			throw WdeException(LogChannel::SCENE, "Trying to draw the Gizmo objects without initializing the renderer.");
		auto& inst = GizmoManager::_gizmoInstance;

		// Set command buffer
		inst->_commandBuffer = &commandBuffer;

		// Render Gizmo for modules
		for (auto& go : gameObjects)
			for (auto& mod : go->getModules())
				mod->drawGizmo(*GizmoManager::_gizmoInstance, commandBuffer);
	}

	void GizmoManager::cleanUp() {
		WDE_PROFILE_FUNCTION();
		if (_gizmoInstance != nullptr)
			_gizmoInstance.reset();
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
