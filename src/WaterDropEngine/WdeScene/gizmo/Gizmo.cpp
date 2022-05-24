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
			auto scene = WaterDropEngine::get().getInstance().getScene();
			_pipelines.at(colorStr)->addDescriptorSet(scene->getDefaultGlobalSet().second);
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
		auto scene = WaterDropEngine::get().getInstance().getScene();
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		                        _pipelines.at(colorStr)->getLayout(), 0, 1,
								&scene->getDefaultGlobalSet().first, 0, nullptr); // Scene data (binding : 0)
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelines.at(colorStr)->getLayout(),
								1, 1, &_pipelinesData.at(colorStr).second.first, 0, nullptr); // Material (binding : 1)
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelines.at(colorStr)->getLayout(),
		                        2, 1, &_positionsSet.first, 0, nullptr); // Bind model (binding : 2)
		_meshes.at("CUBE")->bind(*_commandBuffer); // Bind model data

		// Render GO
		_meshes.at("CUBE")->render();
	}


	// Lines manager
	void Gizmo::resetLinesManager() {
		WDE_PROFILE_FUNCTION();

		// Clear instances
		_linesManagerInstances.clear();
	}

	Gizmo* Gizmo::linesManager(const Color& color) {
		// Create new instance
		LinesManagerInstance inst {};
		inst.color = color;
		if (_linesManagerInstances.empty())
			inst.lastLinesIndex = 0;
		else
			inst.lastLinesIndex = _linesManagerInstances[_linesManagerInstances.size() - 1].linesCount
								+ _linesManagerInstances[_linesManagerInstances.size() - 1].lastLinesIndex;
		inst.linesCount = 0;
		_linesManagerInstances.push_back(inst);

		// Map vertex buffer
		_linesSetData = (resource::Vertex*) _positionsLinesSetBufferVertices->map();

		return this;
	}

	Gizmo* Gizmo::addLine(const glm::vec3& from, const glm::vec3& to) {
		return addLine({from.x, from.y, from.z, 0.0f}, {to.x, to.y, to.z, 0.0f});
	}

	Gizmo* Gizmo::addLine(const glm::vec4& from, const glm::vec4& to) {
		// Add line to vertex buffer
		auto& s = _linesManagerInstances[_linesManagerInstances.size() - 1];
		_linesSetData[(s.linesCount + s.lastLinesIndex) * 2 + 0] = resource::Vertex {glm::vec3{from.x, from.y, from.z}};
		_linesSetData[(s.linesCount + s.lastLinesIndex) * 2 + 1] = resource::Vertex {glm::vec3{to.x, to.y, to.z}};

		// Update instance
		s.linesCount++;

		return this;
	}

	void Gizmo::drawLines() {
		// Unmap vertex buffer
		_positionsLinesSetBufferVertices->unmap();
	}

	void Gizmo::drawLinesInstance(render::CommandBuffer& commandBuffer) {
		WDE_PROFILE_FUNCTION();

		// Update camera descriptor
		auto cam = WaterDropEngine::get().getInstance().getScene()->getActiveCamera();
		if (cam == nullptr)
			return;
		{
			void *data = _positionsLinesSetBuffer->map();
			auto camMod = cam->getModule<CameraModule>();
			GPUGizmoLineCamera dataStruct {};
			dataStruct.camProj = camMod->getProjection();
			dataStruct.camView = camMod->getView();
			memcpy(data, &dataStruct, sizeof(GPUGizmoLineCamera));
			_positionsLinesSetBuffer->unmap();
		}

		// Update color descriptor
		{
			auto* data = (GPUGizmoColorDescriptor*) _linesManagerColors->map();
			uint32_t index = 0;
			for (auto& i : _linesManagerInstances)
				data[index++] = GPUGizmoColorDescriptor {glm::vec4 {i.color._r, i.color._g, i.color._b, i.color._a}};
			_linesManagerColors->unmap();
		}


		// Bind descriptors and pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _linesPipeline->getPipeline());
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _linesPipeline->getLayout(),
		                        0, 1, &_positionsLinesSet.first, 0, nullptr); // Bind global pipeline (binding : 0)

		// Bind vertex buffer
		VkBuffer vertexBuffers[] = { _positionsLinesSetBufferVertices->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		// Draw lines
		uint32_t instanceID = 0;
		for (auto& i : _linesManagerInstances)
			vkCmdDraw(commandBuffer, i.linesCount * 2, 1, i.lastLinesIndex * 2, instanceID++);
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
