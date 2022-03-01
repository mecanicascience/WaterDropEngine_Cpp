#pragma once

#include "Module.hpp"

#include <utility>
#include "../materials/Material.hpp"
#include "../meshes/Mesh.hpp"
#include "../GameObject.hpp"
#include "MeshRendererModule.hpp"

namespace wde::scene {
	/**
	 * Describes a terrain game object handler
	 */
	class TerrainModule : public Module {
		public:
			explicit TerrainModule(GameObject& gameObject);
	};
}
