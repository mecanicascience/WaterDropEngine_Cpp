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
			/**
			 * Create a new terrain
			 * @param gameObject Game object of the corresponding terrain
			 * @param heightMap Heightmap of the terrain data
			 * @param height World height of the terrain
			 * @param precision Number of subdivisions on each side of the terrain
			 * @param gameObjectDimensions World scene game object mesh size of the terrain
			 */
			explicit TerrainModule(GameObject& gameObject, const std::string& heightMap, float height, uint32_t precision, glm::vec4 gameObjectDimensions);
	};
}
