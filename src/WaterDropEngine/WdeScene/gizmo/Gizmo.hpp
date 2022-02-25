#pragma once

#include "../../../wde.hpp"
#include "../../WdeCommon/WdeUtils/Color.hpp"
#include "../gameObjects/materials/Material.hpp"
#include "../gameObjects/meshes/Mesh.hpp"
#include "../gameObjects/materials/GizmoMaterial.hpp"

namespace wde::scene {
#ifdef WDE_ENGINE_MODE_DEBUG
	/**
	 * Stores debug drawing functions
	 */
	class Gizmo {
		public:
			struct GPUGizmoObjectDescriptor {
				glm::mat4 transformWorldSpace;
			};

			struct GPUGizmoLineDescriptor {
				glm::mat4 camView {1.0f};
				glm::mat4 camProj {1.0f};
				glm::vec4 matColor {0.0f};
			};

			~Gizmo() {
				_commandBuffer = nullptr;
				_pipelines.clear();
			}

			/**
			 * Sets the next gizmo shape color
			 * @param color
			 */
			void setColor(const Color& color);


			// Shapes
			/**
			 * Draw a cube
			 * @param center (X, Y, Z) Center cube position
			 * @param rotation (X, Y, Z) Cube game world rotation
			 * @param size  (X, Y, Z) Scale of the cube (default : (1,1,1))
			 */
			void drawCube(const glm::vec3& center, const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& size = glm::vec3 {1.0f, 1.0f, 1.0f});


			// Lines
			/** Create a new line drawing session */
			Gizmo* linesManager(Color color);
			/**
			 * Adds a new line to the drawing session
			 * @param from
			 * @param to
			 */
			Gizmo* addLine(glm::vec3 from, glm::vec3 to);

			/** Draw every lines in the current drawing session */
			void drawLines(render::CommandBuffer& commandBuffer);


			// Parameters
			/** Color of the next shape (default : white) */
			Color _currentColor = Color::WHITE;
			/** Gizmo command buffer */
			render::CommandBuffer* _commandBuffer = nullptr;
			/** Drawing render stage */
			std::pair<int, int> _renderStage {};

			// Render values
			/** Gizmo instance color pipelines */
			std::unordered_map<std::string, std::shared_ptr<Material>> _pipelines {};
			std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshes {};
			/** The current session lines */
			std::vector<std::pair<glm::vec3, glm::vec3>> _lines {};
			std::unordered_map<std::string, std::shared_ptr<render::PipelineGraphics>> _linesPipelines {};

			// Descriptors
			/** Game Objects descriptor set */
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _positionsSet {};
			std::shared_ptr<render::Buffer> _positionsSetBuffer {};

			/** Lines Game Objects descriptor set */
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _positionsLinesSet {};
			std::shared_ptr<render::Buffer> _positionsLinesSetBuffer {};
			std::shared_ptr<render::Buffer> _positionsLinesSetBufferVertices {};
			Vertex* _linesSetData = nullptr;


		private:
			static glm::mat4 getMat(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	};
#endif

#ifdef WDE_ENGINE_MODE_PRODUCTION
	class Gizmo {
		public:
			void setColor(const Color& color) {}
			void drawCube(const glm::vec3& center, const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& size = glm::vec3 {1.0f, 1.0f, 1.0f}) {}
	};
#endif
}
