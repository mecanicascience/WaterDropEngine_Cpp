#include "Material.hpp"

#include <utility>
#include "../../../WaterDropEngine.hpp"

namespace wde::scene {
	Material::Material(std::string name, std::pair<int, int> renderStage, const std::vector<std::string> &shaders)
			: _name(std::move(name)),
			  _renderStage(renderStage),
			  _pipeline(std::make_unique<render::PipelineGraphics>(
					  renderStage,
					  shaders, // Shaders
					  std::vector<VertexInput>{ Vertex::getDescriptions() }, // Vertices
					  render::PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
					  render::PipelineGraphics::Depth::None,    // Do not use depth
					  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
					  VK_POLYGON_MODE_FILL,   // Fill drawn shapes
					  VK_CULL_MODE_NONE)) {  // Disable pipeline culling
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
