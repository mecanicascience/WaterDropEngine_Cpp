#pragma once

#include "Module.hpp"
#include "../../WdeGUI/GUIRenderer.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace wde::scene {
	class TransformModule : public Module {
		public:
			explicit TransformModule(GameObject& gameObject);
			~TransformModule() override;

			void setConfig(const std::string& data);
			void tick() override;
			void drawGUI() override;
			json serialize() override;



			// Getters and setters
			/** Return the corresponding transform matrix : Translation * Ry * Rx * Rz * scale */
			glm::mat4 getTransform() const;



			// Parents and children handling
			/**
			 * Set game object transform parent
			 * @param parent
			 */
			void setParent(TransformModule* parent);
			const TransformModule* getParent() { return _parent; }
			const std::vector<int>& getChildrenIDs() { return _childrenIDs; }

			/**
			 * Extract position, rotation and scale from a transform
			 * @param transform
			 * @param position
			 * @param rotation
			 * @param scale
			 */
			static void decomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);



			// Core parameters
			/** The game object world position */
			glm::vec3 position {0.0f, 0.0f, 0.0f};
			/** The game object world axis rotation */
			glm::vec3 rotation {0.0f, 0.0f, 0.0f};
			/** The game object world scale */
			glm::vec3 scale {1.0f, 1.0f, 1.0f};


		private:
			/** Module transform parent */
			TransformModule* _parent = nullptr;
			/** Module children IDs */
			std::vector<int> _childrenIDs {};
	};
}
