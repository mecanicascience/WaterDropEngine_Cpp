#pragma once

#include "Module.hpp"

#include <utility>
#include "../materials/Material.hpp"

namespace wde::scene {
	/**
	 * Describes a module game object that can hold meshes and materials
	 */
	class MeshRendererModule : public Module {
		public:
			explicit MeshRendererModule(GameObject& gameObject) : Module(gameObject, "Mesh Renderer") { }

			void tick() override { }

			void drawGUI() override {
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
				if (_material == nullptr)
					ImGui::Text(" No material selected.");
				else
					ImGui::Text(" Material name : '%s'.", _material->getName().c_str());
				ImGui::PopFont();
			}


			// Getters and setters
			void setMaterial(const std::shared_ptr<Material>& material) {
				// Remove GO reference
				if (_material != nullptr)
					_material->removeGameObject(_gameObject.getID());

				// Add GO reference
				_material = material;
				_material->addGameObject(_gameObject.getID());
			}


		private:
			// Core data
			/** Selected material of the mesh renderer */
			std::shared_ptr<Material> _material;
	};
}
