#include "TerrainModule.hpp"
#include "../../../WaterDropEngine.hpp"
#include "../materials/ColorMaterial.hpp"
#include "../../../WdeRender/images/Texture2D.hpp"
#include "../../../WdeRender/pipelines/PipelineCompute.hpp"

namespace wde::scene {
	struct PushConstants {
		glm::vec4 readingBounds; // From w,h -> To w,h
		uint32_t readingPointsCount; // Number of points in the mesh
		float terrainHeight; // Height of the terrain in world units
	};

	TerrainModule::TerrainModule(GameObject &gameObject) : Module(gameObject, "Terrain Renderer") {
		// Load heightmap
		auto heightMapTex = std::make_shared<render::Texture2D>("res/terrain/testTerrain/heightmap.png");
		auto imageDescriptor = heightMapTex->createDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// Params
		glm::vec4 dim {0.0, 0.0, 100, 100}; // Mesh dimensions of the terrain + offset
		uint32_t readingPointsCount = 100; // Numbers of mesh points on each side of the terrain
		float terrainHeight = 10;


		// Create buffer from heightmap
		auto terrainMesh = WaterDropEngine::get().getInstance().getScene().createMesh<Mesh>("Terrain mesh");
		terrainMesh->getVertexBuffer() = std::make_shared<render::Buffer>(sizeof(Vertex) * readingPointsCount * readingPointsCount,
												 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		terrainMesh->getIndexBuffer() = std::make_shared<render::Buffer>(6 * sizeof(uint32_t) * (readingPointsCount - 1) * (readingPointsCount - 1),
												 VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		terrainMesh->setIndexCount(6 * (readingPointsCount - 1) * (readingPointsCount - 1));

		// Create compute descriptor set
		std::pair<VkDescriptorSet, VkDescriptorSetLayout> descriptorSet;
		render::DescriptorBuilder::begin()
				.bind_image(0, &imageDescriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT)
				.bind_buffer(1, &terrainMesh->getVertexBuffer()->getBufferInfo(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.bind_buffer(2, &terrainMesh->getIndexBuffer()->getBufferInfo(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.build(descriptorSet.first, descriptorSet.second);

		// Create compute pipeline
		render::PipelineCompute computePipeline {"res/shaders/common/terrain/terrainFromHeight.comp"};
		computePipeline.addDescriptorSet(descriptorSet.second);
		computePipeline.addPushConstants(sizeof(PushConstants));
		computePipeline.initialize();

		// Fill buffer from heightmap
		render::CommandBuffer commandBuffer {true};

		PushConstants cs {};
		cs.readingBounds = dim;
		cs.readingPointsCount = readingPointsCount;
		cs.terrainHeight = terrainHeight;

		computePipeline.bind(commandBuffer);
		computePipeline.setPushConstants(&cs);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline.getLayout(), 0, 1, &descriptorSet.first, 0, nullptr);
		vkCmdDispatch(commandBuffer, std::ceil(((float) readingPointsCount) / 32.0), std::ceil(((float) readingPointsCount) / 32.0), 1);
		commandBuffer.end();
		commandBuffer.submitIdle();
		commandBuffer.waitForQueueIdle();

		// Load terrain material and mesh
		auto meshModule = _gameObject.addModule<MeshRendererModule>();
		auto terrainMaterial = WaterDropEngine::get().getInstance().getScene().createMaterial<ColorMaterial>(
				std::pair<int, int>{0, 0}, Color::WHITE, VK_POLYGON_MODE_LINE);
		meshModule->setMaterial(terrainMaterial);
		meshModule->setMesh(terrainMesh);
	}
}
