#pragma once

#include "Module.hpp"
#include "../../WdeGUI/GUIRenderer.hpp"

namespace wde::scene {
	class TransformModule : public Module {
		public:
			explicit TransformModule(GameObject& gameObject);
			~TransformModule() override;

			void setConfig(const std::string& data);
			void tick() override;
			void drawGUI() override;



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



			// Core parameters
			/** The game object world position */
			glm::vec3 position {0.0f, 0.0f, 0.0f};
			/** The game object world axis rotation */
			glm::vec3 rotation {0.0f, 0.0f, 0.0f};
			/** The game object world scale */
			glm::vec3 scale {1.0f, 1.0f, 1.0f};

			/** True if the transform module has changed since last tick */
			bool changed = true;


		private:
			/** Module transform parent */
			TransformModule* _parent = nullptr;
			/** Module children IDs */
			std::vector<int> _childrenIDs {};
			/** Transform matrix from last tick */
			glm::mat4 _lastTransform {1.0f};
	};
}
