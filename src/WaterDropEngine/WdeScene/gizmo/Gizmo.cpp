#include "Gizmo.hpp"
#include "../../WaterDropEngine.hpp"

#ifdef WDE_ENGINE_MODE_DEBUG
namespace wde::scene {
	void Gizmo::setColor(const Color& color) {
		WDE_PROFILE_FUNCTION();
		if (!_pipelines.contains(color.toString())) {
			// Create corresponding pipeline
			_pipelines.emplace(color.toString(),
			                   std::make_shared<GizmoMaterial>(_renderStage, color, VK_POLYGON_MODE_LINE));

			// Add pipeline object descriptor
			_pipelines.at(color.toString())->addDescriptorSet(_positionsSet);
			_pipelines.at(color.toString())->createMaterial(); // Setup pipeline
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
		WaterDropEngine::get().getInstance().getPipeline().bind(*_commandBuffer, _pipelines.at(_currentColor.toString()).get()); // Scene data (binding : 0)
		_pipelines.at(_currentColor.toString())->bind(*_commandBuffer); // Material data (binding : 1)
		_meshes.at("CUBE")->bind(*_commandBuffer); // Bind model data
		vkCmdBindDescriptorSets(*_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelines.at(_currentColor.toString())->getPipeline().getLayout(),
								2, 1, &_positionsSet.first, 0, nullptr); // Bind model (binding : 2)

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
			_linesPipelines.emplace(color.toString(),
                   std::make_shared<render::PipelineGraphics>(_renderStage, std::vector<std::string>{"res/shaders/common/gizmo/gizmoLines.vert", "res/shaders/common/gizmo/gizmoLines.frag"},
															  std::vector<scene::VertexInput>{ Vertex::getDescriptions() },
															  render::PipelineGraphics::Mode::Polygon, render::PipelineGraphics::Depth::ReadWrite,
															  VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE));

			// Add pipeline object descriptor
			_linesPipelines.at(color.toString())->addDescriptorSet(_positionsLinesSet.second);
			_linesPipelines.at(color.toString())->initialize(); // Setup pipeline
		}

		// Map vertex buffer
		_linesSetData = (Vertex*) _positionsLinesSetBufferVertices->map();

		return this;
	}

	Gizmo* Gizmo::addLine(const glm::vec3& from, const glm::vec3& to) {
		// Add line to vertex buffer
		_linesSetData[_lines.size() * 2 + 0] = Vertex {from};
		_linesSetData[_lines.size() * 2 + 1] = Vertex {to};

		// Add lines to list
		_lines.emplace_back(from, to);

		return this;
	}

	Gizmo* Gizmo::addLine(const glm::vec4& from, const glm::vec4& to) {
		// Add line to vertex buffer
		_linesSetData[_lines.size() * 2 + 0] = Vertex {glm::vec3{from.x, from.y, from.z}};
		_linesSetData[_lines.size() * 2 + 1] = Vertex {glm::vec3{to.x, to.y, to.z}};

		// Add lines to list
		_lines.emplace_back(from, to);

		return this;
	}

	void Gizmo::drawLines(render::CommandBuffer& commandBuffer) {
		WDE_PROFILE_FUNCTION();

		// Unmap vertex buffer
		_positionsLinesSetBufferVertices->unmap();

		// Update descriptor
		void *data = _positionsLinesSetBuffer->map();
		auto cam = WaterDropEngine::get().getInstance().getScene().getActiveCamera();
		if (cam == nullptr)
			throw WdeException(LogChannel::SCENE, "No active camera in scene.");
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
		_commandBuffer = nullptr;
		_pipelines.clear();
	}
}
#endif
