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
}
#endif
