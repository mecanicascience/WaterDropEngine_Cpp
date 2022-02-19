#pragma once

#include "Module.hpp"

#include <utility>
#include "../materials/Material.hpp"
#include "../meshes/Mesh.hpp"
#include "../GameObject.hpp"

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
				// Mesh
                if (_mesh == nullptr)
                    ImGui::Text(" No mesh selected.");
                else
                    ImGui::Text(" Mesh name : '%s'.", _mesh->getName().c_str());
                ImGui::Separator();

				// Material
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
					_material->removeGameObject(static_cast<int>(_gameObject.getID()));

				// Add GO reference
				_material = material;
				_material->addGameObject(static_cast<int>(_gameObject.getID()));
			}
            std::shared_ptr<Material>& getMaterial() { return _material; }
			void setMesh(const std::shared_ptr<Mesh>& mesh) { _mesh = mesh; }
            std::shared_ptr<Mesh>& getMesh() { return _mesh; }


		private:
			// Core data
			/** Selected game object mesh */
			std::shared_ptr<Mesh> _mesh;
			/** Selected material of the mesh renderer */
			std::shared_ptr<Material> _material;
	};
}
