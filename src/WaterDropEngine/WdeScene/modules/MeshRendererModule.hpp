#pragma once

#include "Module.hpp"

#include <utility>
#include "../../WdeResourceManager/resources/Material.hpp"
#include "../../WdeResourceManager/resources/Mesh.hpp"
#include "../GameObject.hpp"

namespace wde::scene {
	/**
	 * Describes a module game object that can hold meshes and materials
	 */
	class MeshRendererModule : public Module {
		public:
			explicit MeshRendererModule(GameObject& gameObject);
			explicit MeshRendererModule(GameObject& gameObject, const std::string& data);
			void drawGUI() override;
			json serialize() override;


			// Getters and setters
			void setMaterial(const std::shared_ptr<resource::Material>& material) { _material = material; }
			const std::shared_ptr<resource::Material>& getMaterial() const { return _material; }
			void setMesh(const std::shared_ptr<resource::Mesh>& mesh) { _mesh = mesh; }
            const std::shared_ptr<resource::Mesh>& getMesh() const { return _mesh; }


		private:
			// Core data
			/** Selected game object mesh */
			std::shared_ptr<resource::Mesh> _mesh;
			/** Selected material of the mesh renderer */
			std::shared_ptr<resource::Material> _material;

			// Material data
			std::string _meshName;
			std::string _materialName;
	};
}
