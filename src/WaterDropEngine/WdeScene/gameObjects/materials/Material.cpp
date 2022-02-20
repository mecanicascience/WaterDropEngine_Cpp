#include "Material.hpp"

#include <utility>
#include "../../../WaterDropEngine.hpp"

namespace wde::scene {
	Material::Material(std::string name, std::pair<int, int> renderStage, const std::vector<std::string> &shaders, VkPolygonMode polygonMode)
			: _name(std::move(name)),
			  _renderStage(renderStage),
			  _pipeline(std::make_unique<render::PipelineGraphics>(
					  renderStage,
					  shaders, // Shaders
					  std::vector<VertexInput>{ Vertex::getDescriptions() }, // Vertices
					  render::PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
					  render::PipelineGraphics::Depth::ReadWrite,    // Do not use depth
					  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
					  polygonMode,   // Fill drawn shapes
					  VK_CULL_MODE_BACK_BIT,
					  VK_FRONT_FACE_COUNTER_CLOCKWISE)) {  // Disable pipeline culling
		static int materialID = 0;
		_materialID = materialID++;
	}

	Material::~Material() {
		_pipeline.reset();
	}


	void Material::createMaterial() {
		auto& pipeline = WaterDropEngine::get().getInstance().getPipeline();

		// Add descriptors if created
		_pipeline->addDescriptorSet(pipeline.getGlobalSet().second);
		if (_materialSet.first != VkDescriptorSet {})
			_pipeline->addDescriptorSet(_materialSet.second);

		// Initialize pipeline
		_pipeline->initialize();
	}
}
