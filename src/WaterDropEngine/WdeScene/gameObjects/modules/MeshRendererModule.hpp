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
			explicit MeshRendererModule(GameObject& gameObject) : Module(gameObject, "Mesh Renderer", ICON_FA_GHOST) {}

			void drawGUI() override {
				ImGui::PushFont(ImGui::GetIO().FontDefault);
				// Mesh
                if (_mesh == nullptr)
                    ImGui::Text(" No mesh selected.");
                else
                    ImGui::Text(" Mesh name : \"%s\".", _mesh->getName().c_str());

				// Material
				if (_material == nullptr)
					ImGui::Text(" No material selected.");
				else
					ImGui::Text(" Material name : \"%s\".", _material->getName().c_str());
				ImGui::PopFont();
			}


			// Getters and setters
			void setMaterial(const std::shared_ptr<Material>& material) { _material = material; }
			const std::shared_ptr<Material>& getMaterial() const { return _material; }
			void setMesh(const std::shared_ptr<Mesh>& mesh) { _mesh = mesh; }
            const std::shared_ptr<Mesh>& getMesh() const { return _mesh; }


		private:
			// Core data
			/** Selected game object mesh */
			std::shared_ptr<Mesh> _mesh;
			/** Selected material of the mesh renderer */
			std::shared_ptr<Material> _material;
	};
}
