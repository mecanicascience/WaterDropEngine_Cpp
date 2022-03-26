#include "Material.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::resource {
	Material::Material(const std::string &path) : Resource(path, ResourceType::MATERIAL) {
		WDE_PROFILE_FUNCTION();
		auto matData = json::parse(WdeFileUtils::readFile(path));
		if (matData["type"] != "material")
			throw WdeException(LogChannel::RES, "Trying to create a material from a non-material description.");

		// Setup material
		{
			// Set material data
			_name = matData["name"];
			static int materialID = 0;
			_materialID = materialID++;
			_renderStage = std::pair<int, int>(matData["data"]["renderStage"]["pass"].get<int>(), matData["data"]["renderStage"]["subpass"].get<int>());

			// Get shaders absolute reference
			std::vector<std::string> shadersLoc {};
			auto scenePath = WaterDropEngine::get().getInstance().getScene().getPath();
			for (auto& s : matData["data"]["shaders"])
				shadersLoc.push_back(scenePath + "data/shaders/" + s.get<std::string>());

			// Get polygon mode
			VkPolygonMode polygonMode;
			if (matData["data"]["polygonMode"] == "fill")
				polygonMode = VK_POLYGON_MODE_FILL;
			else if (matData["data"]["polygonMode"] == "line")
				polygonMode = VK_POLYGON_MODE_LINE;
			else if (matData["data"]["polygonMode"] == "point")
				polygonMode = VK_POLYGON_MODE_POINT;

			// Create pipeline
			_pipeline = std::make_unique<render::PipelineGraphics>(
					_renderStage,
					shadersLoc, // Shaders
					std::vector<resource::VertexInput>{ resource::Vertex::getDescriptions() }, // Vertices
					render::PipelineGraphics::Mode::Polygon, // Draw one polygon at a time
					render::PipelineGraphics::Depth::ReadWrite, // Read and write to depth
					VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // Draw shapes as triangles
					polygonMode,   // Fill drawn shapes
					VK_CULL_MODE_BACK_BIT,
					VK_FRONT_FACE_COUNTER_CLOCKWISE);
		}

		// Create descriptor and resources
		{
			// Create descriptor builder
			auto descBuilder = render::DescriptorBuilder::begin();

			// Create descriptor resources
			uint32_t set = 0;
			for (auto& setData : matData["data"]["descriptor"]) {
				// Get stages visibles
				uint32_t stagesMask = 0;
				for (auto& st : setData["stages"]) {
					if (st == "frag")
						stagesMask |= VK_SHADER_STAGE_FRAGMENT_BIT;
					else if (st == "vert")
						stagesMask |= VK_SHADER_STAGE_VERTEX_BIT;
					else if (st == "compute")
						stagesMask |= VK_SHADER_STAGE_COMPUTE_BIT;
				}

				if (setData["type"] == "image") {
					auto imageType = json::parse(WdeFileUtils::readFile(WaterDropEngine::get().getInstance().getScene().getPath() + "data/textures/" + setData["data"]["path"].get<std::string>()));

					if (imageType["data"]["type"] == "2D") {
						// Create image descriptor
						auto imageDescriptor = WaterDropEngine::get().getResourceManager().load<resource::Texture2D>(
								WaterDropEngine::get().getInstance().getScene().getPath() + "data/textures/" + setData["data"]["path"].get<std::string>()
						)->createDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

						// Bind image
						descBuilder.bind_image(set, &imageDescriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stagesMask);
					}
					else if (imageType["data"]["type"] == "cube") {
						// Create image descriptor
						auto imageDescriptor = WaterDropEngine::get().getResourceManager().load<resource::TextureCube>(
								WaterDropEngine::get().getInstance().getScene().getPath() + "data/textures/" + setData["data"]["path"].get<std::string>()
						)->createDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

						// Bind image
						descBuilder.bind_image(set, &imageDescriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stagesMask);
					}
					else
						throw WdeException(LogChannel::RES, "Trying to create a descriptor set image a not implemented image type " + setData["type"].get<std::string>());
				}
				else
					throw WdeException(LogChannel::RES, "Trying to create a descriptor set from a not implemented type " + setData["type"].get<std::string>());

				set++;
			}

			// Build descriptor
			descBuilder.build(_materialSet.first, _materialSet.second);
		}

		// Create material
		{
			auto& pipeline = WaterDropEngine::get().getInstance().getPipeline();

			// Add material descriptors if created
			_pipeline->addDescriptorSet(pipeline.getGlobalSet().second);
			if (_materialSet.first != VkDescriptorSet {})
				_pipeline->addDescriptorSet(_materialSet.second);

			// Add other descriptors
			if (_materialSet != std::pair<VkDescriptorSet, VkDescriptorSetLayout>{})
				_pipeline->addDescriptorSet(_materialSet.second);

			// Initialize pipeline
			_pipeline->initialize();
		}
	}

	void Material::bind(render::CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();

		// Bind material descriptor
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		                        _pipeline->getLayout(), 1, 1, &_materialSet.first, 0, nullptr);

		// Bind pipeline
		_pipeline->bind(commandBuffer);
	}
}
