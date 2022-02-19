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
		auto& pipeline = WaterDropEngine::get().getInstance().getPipeline();

		// Create material descriptor set
		_materialData = std::make_unique<render::Buffer>(sizeof(GPUMaterialData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		render::DescriptorBuilder::begin()
					.bind_buffer(0, &_materialData->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(_materialSet.first, _materialSet.second);

		// Add descriptors
		_pipeline->addDescriptorSet(pipeline.getGlobalSet().second);
		_pipeline->addDescriptorSet(_materialSet.second);

		// Initialize pipeline
		_pipeline->initialize();
	}

	Material::~Material() {
		_pipeline.reset();
	}
}
