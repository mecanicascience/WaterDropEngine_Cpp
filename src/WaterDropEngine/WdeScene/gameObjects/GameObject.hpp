#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeCore/Structure/Observer.hpp"
#include "modules/TransformModule.hpp"
#include "../../WdeRender/commands/CommandBuffer.hpp"
#include "../../WdeRender/buffers/Buffer.hpp"
#include "../../WdeRender/descriptors/DescriptorBuilder.hpp"

namespace wde::scene {
	/**
	 * Handles a game object
	 */
	class GameObject : public NonCopyable {
		public:
			struct GPUGameObjectData {
				/** Game object world space position */
				glm::mat4 transformWorldSpace {1.0f};
				/** Game object collision sphere */
				glm::vec4 collisionSphere {0.0f};
			};


			// Core functions
			/**
			 * Create a new game object with a transform module (default : 0, 0, 0)
			 * @param id Unique GO ID
			 * @param name
			 * @param isStatic
			 */
			GameObject(uint32_t id, std::string name, bool isStatic);
			~GameObject() override;
			void tick();
			void drawGUI();


			// Getters and setters
			uint32_t getID() const { return _id; }
			void setSelected(bool selected) { _isSelected = selected; }
			bool isSelected() const { return _isSelected; }
			bool isStatic() const { return _isStatic; }
			const std::vector<std::unique_ptr<Module>>& getModules() const { return _modules; }


			// Modules handlers
			template<typename T, typename ...Args>
			T* addModule(Args ...args) {
				_modules.push_back(std::make_unique<T>(*this, args...));
				return static_cast<T*>(_modules[_modules.size() - 1].get());
			}

			template<typename T>
			T* getModule() {
			    // Iterate over every module
				for (auto& mod : _modules)
					if (dynamic_cast<T*>(mod.get()) != nullptr)
						return static_cast<T*>(mod.get());
				// Not found
				return nullptr;
			}


			// Public GO data
			/** True if the game object is active */
			bool active = true;
			/** Name of the game object */
			std::string name;
			/** Transform module of the game object */
			TransformModule* transform;


		private:
			/** GO description data */
			uint32_t _id;
			/** GO Modules */
			std::vector<std::unique_ptr<Module>> _modules;
			/** True if the game object is static (cannot be changed) */
			bool _isStatic;
			/** If true, this object will record from the input engine if it has a player controller */
			bool _isSelected = false;
	};
}

