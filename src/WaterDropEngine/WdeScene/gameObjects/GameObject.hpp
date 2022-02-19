#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeCore/Structure/Observer.hpp"
#include "modules/TransformModule.hpp"
#include "../../WdeRender/commands/CommandBuffer.hpp"
#include "../../WdeRender/buffers/Buffer.hpp"
#include "../../WdeRender/descriptors/DescriptorBuilder.hpp"
#include "materials/Material.hpp"

namespace wde::scene {
	/**
	 * Handles a game object
	 */
	class GameObject {
		public:
			struct GPUGameObjectData {
				/** Game object world space position */
				glm::mat4 transformWorldSpace {1.0f};
			};


			// Core functions
			/**
			 * Create a new game object with a transform module (default : 0, 0, 0)
			 * @param id Unique GO ID
			 * @param name
			 */
			GameObject(uint32_t id, std::string name)
					: _id(id), name(std::move(name)) {
				// Add default transform module
				transform = addModule<TransformModule>();

				// Create game object set
				_gameObjectData = std::make_unique<render::Buffer>(sizeof(GPUGameObjectData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
				render::DescriptorBuilder::begin()
						.bind_buffer(0, &_gameObjectData->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
						.build(_gameObjectSet.first, _gameObjectSet.second);
			}
			static VkDescriptorSetLayout* getGeneralGameObjectDescriptorSet() {
				static std::pair<VkDescriptorSet, VkDescriptorSetLayout> _objectGeneralSet;
				static bool defined = false;
				if (defined)
					return &_objectGeneralSet.second;

				// Create general global game object set
				render::Buffer generalGameObjectData {sizeof(GPUGameObjectData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT};
				render::DescriptorBuilder::begin()
						.bind_buffer(0, &generalGameObjectData.getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
						.build(_objectGeneralSet.first, _objectGeneralSet.second);

				defined = true;
				return &_objectGeneralSet.second;
			}


			~GameObject() {
				transform = nullptr;
				_modules.clear();
			}

			void tick() {
				// Tick for modules
				for (auto& mod : _modules)
					mod->tick();

				// If transform changed, update transform buffer
				if (transform->changed) {
					transform->changed = false;

					// New data
					GPUGameObjectData objData {};
					objData.transformWorldSpace = transform->getTransform();

					// Map data
					void *data = _gameObjectData->map();
					memcpy(data, &objData, sizeof(GPUGameObjectData));
					_gameObjectData->unmap();
				}
			}

			/**
			 * Bind the game object descriptor to the pipeline
			 * @param buffer
			 * @param material
			 */
			void bind(render::CommandBuffer &commandBuffer, std::shared_ptr<Material> material) {
				// Bind mesh set descriptor
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				                        material->getPipeline().getLayout(), 2, 1, &_gameObjectSet.first, 0, nullptr);
			}

			void drawGUI() {
				WDE_PROFILE_FUNCTION();
				// Render module top
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::Text("%s", (std::to_string(_id) + " - " + name).c_str());
				ImGui::PopFont();
				ImGui::Dummy(ImVec2(0.0f, 3.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				// Render Modules GUI
				int count = 0;
				for(auto& module : _modules) {
					// Add small space between modules
					if (count != 0)
						ImGui::Dummy(ImVec2(0.0f, 18.0f));
					count++;

					// Render module
					ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
					ImGui::Dummy(ImVec2(6.0f, 0.0f));
					ImGui::SameLine();
					if (ImGui::CollapsingHeader(module->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
						ImGui::Dummy(ImVec2(0.0f, 1.0f));
						ImGui::PopFont();
						// Render header content
						module->drawGUI();
					}
					else
						ImGui::PopFont();
				}
			}


			// Getters and setters
			uint32_t getID() const { return _id; }


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
				logger::log(LogLevel::WARN, LogChannel::SCENE) << "Trying to get an undefined module." << logger::endl;
				// Try to add this module of this type with no parameters
				return addModule<T>();
			}

            template<typename T>
            bool hasModule() {
                for (auto& mod : _modules)
                    if (static_cast<T*>(mod.get()) != nullptr)
                        return true;
                return false;
            }


			// Public GO data
			/** Name of the game object */
			std::string name;
			/** Transform module of the game object */
			TransformModule* transform;


		private:
			// GO description data
			uint32_t _id;
			// GO Modules
			std::vector<std::unique_ptr<Module>> _modules;

			// GO Descriptor Set
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _gameObjectSet;
			std::unique_ptr<render::Buffer> _gameObjectData;
	};
}

