#include "Gizmo.hpp"
#include "../../WaterDropEngine.hpp"

#ifdef WDE_ENGINE_MODE_DEBUG
namespace wde::scene {
	void Gizmo::setColor(const Color& color) {
		WDE_PROFILE_FUNCTION();
		auto colorStr = color.toString();
		if (!_pipelines.contains(colorStr)) {
			// Create corresponding pipeline
			auto path = WaterDropEngine::get().getInstance().getScene()->getPath();
			_pipelines.emplace(colorStr,
			   std::make_unique<render::PipelineGraphics>(
					   _renderStage, std::vector<std::string>{path + "data/shaders/engine/gizmo/gizmo.vert", path + "data/shaders/engine/gizmo/gizmo.frag"},
                       std::vector<resource::VertexInput>{ resource::Vertex::getDescriptions() },
                       render::PipelineGraphics::Mode::Polygon, render::PipelineGraphics::Depth::ReadWrite,
                       VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE));

			// Create color descriptor
			_pipelinesData.emplace(colorStr, std::pair<std::unique_ptr<render::Buffer>, std::pair<VkDescriptorSet, VkDescriptorSetLayout>> {});
			_pipelinesData.at(colorStr).first = std::make_unique<render::Buffer>(sizeof(GPUGizmoColorDescriptor), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			{
				GPUGizmoColorDescriptor matData {};
				matData.color = glm::vec4(color._r, color._g, color._b, 0.0);

				void* data = _pipelinesData.at(colorStr).first->map();
				memcpy(data, &matData, sizeof(GPUGizmoColorDescriptor));
				_pipelinesData.at(colorStr).first->unmap();
			}

			// Create descriptor set
			render::DescriptorBuilder::begin()
						.bind_buffer(0, *_pipelinesData.at(color.toString()).first, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.build(_pipelinesData.at(colorStr).second.first, _pipelinesData.at(colorStr).second.second);

			// Add pipeline object descriptor
			_pipelines.at(colorStr)->addDescriptorSet(WaterDropEngine::get().getInstance().getPipeline().getGlobalSet().second);
			_pipelines.at(colorStr)->addDescriptorSet(_positionsSet.second);
			_pipelines.at(colorStr)->addDescriptorSet(_pipelinesData.at(colorStr).second.second);
			_pipelines.at(colorStr)->initialize(); // Setup pipeline
		}
		_currentColor = color;
	}


	// Shapes
	void Gizmo::drawCube(const glm::vec3& center, const glm::vec3& rotation, const glm::vec3& size) {
		WDE_PROFILE_FUNCTION();
		// Update data buffer
		void *data = _positionsSetBuffer->map();
		GPUGizmoObjectDescriptor dataStruct {};
		dataStruct.transformWorldSpace = getMat(center, rotation, size);
		memcpy(data, &dataStruct, sizeof(GPUGizmoObjectDescriptor));
		_positionsSetBuffer->unmap();

		// Bind pipelines and descriptors
		auto colorStr = _currentColor.toString();
		_pipelines.at(colorStr)->bind(*_commandBuffer); // Bind pipeline
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		                        _pipelines.at(colorStr)->getLayout(), 0, 1,
								&WaterDropEngine::get().getInstance().getPipeline().getGlobalSet().first, 0, nullptr); // Scene data (binding : 0)
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelines.at(colorStr)->getLayout(),
								1, 1, &_pipelinesData.at(colorStr).second.first, 0, nullptr); // Material (binding : 1)
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelines.at(colorStr)->getLayout(),
		                        2, 1, &_positionsSet.first, 0, nullptr); // Bind model (binding : 2)
		_meshes.at("CUBE")->bind(*_commandBuffer); // Bind model data


		// Render GO
		_meshes.at("CUBE")->render();
	}


	// Lines manager
	Gizmo* Gizmo::linesManager(const Color& color) {
		WDE_PROFILE_FUNCTION();
		// Delete last session
		_lines.clear();
		_linesSetData = nullptr;

		// Set current color
		_currentColor = color;

		// Create line pipeline
		if (!_linesPipelines.contains(color.toString())) {
			// Create corresponding pipeline
			auto path = WaterDropEngine::get().getInstance().getScene()->getPath();
			_linesPipelines.emplace(color.toString(),
                   std::make_shared<render::PipelineGraphics>(_renderStage,
						  std::vector<std::string>{path + "data/shaders/engine/gizmo/gizmoLines.vert", path + "data/shaders/engine/gizmo/gizmoLines.frag"},
						  std::vector<resource::VertexInput>{ resource::Vertex::getDescriptions() },
						  render::PipelineGraphics::Mode::Polygon, render::PipelineGraphics::Depth::ReadWrite,
						  VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE));

			// Add pipeline object descriptor
			_linesPipelines.at(color.toString())->addDescriptorSet(_positionsLinesSet.second);
			_linesPipelines.at(color.toString())->initialize(); // Setup pipeline
		}

		// Map vertex buffer
		_linesSetData = (resource::Vertex*) _positionsLinesSetBufferVertices->map();

		return this;
	}

	Gizmo* Gizmo::addLine(const glm::vec3& from, const glm::vec3& to) {
		// Add line to vertex buffer
		_linesSetData[_lines.size() * 2 + 0] = resource::Vertex {from};
		_linesSetData[_lines.size() * 2 + 1] = resource::Vertex {to};

		// Add lines to list
		_lines.emplace_back(from, to);

		return this;
	}

	Gizmo* Gizmo::addLine(const glm::vec4& from, const glm::vec4& to) {
		// Add line to vertex buffer
		_linesSetData[_lines.size() * 2 + 0] = resource::Vertex {glm::vec3{from.x, from.y, from.z}};
		_linesSetData[_lines.size() * 2 + 1] = resource::Vertex {glm::vec3{to.x, to.y, to.z}};

		// Add lines to list
		_lines.emplace_back(from, to);

		return this;
	}

	void Gizmo::drawLines(render::CommandBuffer& commandBuffer) {
		WDE_PROFILE_FUNCTION();

		// Unmap vertex buffer
		_positionsLinesSetBufferVertices->unmap();

		// Update descriptor
		auto cam = WaterDropEngine::get().getInstance().getScene()->getActiveCamera();
		if (cam == nullptr) {
			return;
		}
		void *data = _positionsLinesSetBuffer->map();
		auto camMod = cam->getModule<CameraModule>();
		GPUGizmoLineDescriptor dataStruct {};
		dataStruct.camProj = camMod->getProjection();
		dataStruct.camView = camMod->getView();
		dataStruct.matColor = glm::vec4(_currentColor._r, _currentColor._g, _currentColor._b, _currentColor._a);
		memcpy(data, &dataStruct, sizeof(GPUGizmoLineDescriptor));
		_positionsLinesSetBuffer->unmap();

		// Bind descriptors and pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _linesPipelines.at(_currentColor.toString())->getPipeline());
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _linesPipelines.at(_currentColor.toString())->getLayout(),
		                        0, 1, &_positionsLinesSet.first, 0, nullptr); // Bind description pipeline (binding : 0)

		// Draw lines
		VkBuffer vertexBuffers[] = { _positionsLinesSetBufferVertices->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdDraw(commandBuffer, _lines.size() * 2, 1, 0, 0);
	}



	// Helpers
	glm::mat4 Gizmo::getMat(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		auto mat = glm::mat4{
				{
						scale.x * (c1 * c3 + s1 * s2 * s3),
						scale.x * (c2 * s3),
						scale.x * (c1 * s2 * s3 - c3 * s1),
						0.0f,
				},
				{
						scale.y * (c3 * s1 * s2 - c1 * s3),
						scale.y * (c2 * c3),
						scale.y * (c1 * c3 * s2 + s1 * s3),
						0.0f,
				},
				{
						scale.z * (c2 * s1),
						scale.z * (-s2),
						scale.z * (c1 * c2),
						0.0f,
				},
				{position.x, position.y, position.z, 1.0f}
		};
		return mat;
	}

	Gizmo::~Gizmo()  {
		WDE_PROFILE_FUNCTION();
		_commandBuffer = nullptr;
		_pipelines.clear();
	}
}
#endif
