#include "TerrainModule.hpp"
#include "../../../WaterDropEngine.hpp"
#include "../materials/ColorMaterial.hpp"
#include "../../../WdeRender/images/Texture2D.hpp"
#include "../../../WdeRender/pipelines/PipelineCompute.hpp"
#include "../../../../../app/examples/05-Terrain/ShadowMaterial.hpp"

namespace wde::scene {
	struct PushConstants {
		glm::vec4 readingBounds; // From w,h -> To w,h
		uint32_t readingPointsCount; // Number of points in the mesh
		float terrainHeight; // Height of the terrain in world units
	};

	TerrainModule::TerrainModule(GameObject &gameObject, const std::string& heightMap, float height, uint32_t precision, glm::vec4 gameObjectDimensions)
			: Module(gameObject, "Terrain Renderer", ICON_FA_MAP) {
		/*// Create buffer from heightmap
		auto terrainMesh = WaterDropEngine::get().getInstance().getScene().createMesh<Mesh>("Terrain mesh");
		terrainMesh->getVertexBuffer() = std::make_shared<render::Buffer>(sizeof(Vertex) * precision * precision,
												 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		terrainMesh->getIndexBuffer() = std::make_shared<render::Buffer>(6 * sizeof(uint32_t) * (precision - 1) * (precision - 1),
												 VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		terrainMesh->setIndexCount(6 * (precision - 1) * (precision - 1));


		// Create compute descriptor set
		auto texture = std::make_shared<render::Texture2D>(heightMap);
		auto imageDescriptor = texture->createDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		std::pair<VkDescriptorSet, VkDescriptorSetLayout> descriptorSet;
		render::DescriptorBuilder::begin()
				.bind_image(0, &imageDescriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT)
				.bind_buffer(1, *terrainMesh->getVertexBuffer(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.bind_buffer(2, *terrainMesh->getIndexBuffer(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.build(descriptorSet.first, descriptorSet.second);


		// Create compute pipeline
		render::PipelineCompute computePipeline {"res/shaders/tests/terrain/terrainFromHeight.comp"};
		computePipeline.addDescriptorSet(descriptorSet.second);
		computePipeline.addPushConstants(sizeof(PushConstants));
		computePipeline.initialize();


		// Fill buffer from heightmap
		render::CommandBuffer commandBuffer {true};
		computePipeline.bind(commandBuffer);

		PushConstants cs {};
		cs.readingBounds = gameObjectDimensions;
		cs.readingPointsCount = precision;
		cs.terrainHeight = height;
		computePipeline.setPushConstants(&cs);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline.getLayout(), 0, 1, &descriptorSet.first, 0, nullptr);
		vkCmdDispatch(commandBuffer, std::ceil(((float) precision) / 32.0), std::ceil(((float) precision) / 32.0), 1);

		commandBuffer.end();
		commandBuffer.submitIdle();
		commandBuffer.waitForQueueIdle();


		// Load terrain material and mesh
		auto meshModule = _gameObject.addModule<MeshRendererModule>();
		auto terrainMaterial = WaterDropEngine::get().getInstance().getScene().createMaterial<ShadowMaterial>(std::pair<int, int>{0, 0}, Color::WHITE, VK_POLYGON_MODE_FILL);
		meshModule->setMaterial(terrainMaterial);
		meshModule->setMesh(terrainMesh);*/
	}
}
