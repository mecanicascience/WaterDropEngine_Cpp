#pragma once

#include "Material.hpp"
#include "../../../WdeRender/descriptors/DescriptorBuilder.hpp"
#include "../../../WdeRender/images/Texture2D.hpp"

namespace wde::scene {
	class TextureMaterial : public Material {
		public:
			/**
			 * Create a new uniformly colored material
			 * @param renderStage
			 * @param path
			 * @param polygonMode (default : fill)
			 */
			explicit TextureMaterial(std::pair<int, int> renderStage, const std::string& path, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL)
					: Material("Texture Material " + path, renderStage, {
					"res/shaders/common/texture/unlit.vert",
					"res/shaders/common/texture/unlit.frag"
			}, polygonMode) {
				// Create texture
				_texture = std::make_unique<render::Texture2D>(path);

				// Create descriptor set
				const VkDescriptorImageInfo imageDescriptor = _texture->createDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				render::DescriptorBuilder::begin()
						.bind_image(0, &imageDescriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.build(_materialSet.first, _materialSet.second);

				// Create material
				createMaterial();
			}


		private:
			std::unique_ptr<render::Texture2D> _texture {};
	};
}

