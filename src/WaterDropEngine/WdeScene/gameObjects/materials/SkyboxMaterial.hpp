#pragma once

#include "Material.hpp"
#include "../../../WdeRender/descriptors/DescriptorBuilder.hpp"
#include "../../../WdeRender/images/Texture2D.hpp"
#include "../../../WdeRender/images/TextureCube.hpp"

namespace wde::scene {
	class SkyboxMaterial : public Material {
		public:
			/**
			 * Create a skybox material
			 * @param renderStage
			 * @param path
			 * @param polygonMode (default : fill)
			 */
			explicit SkyboxMaterial(std::pair<int, int> renderStage, const std::string& path, const std::string& extension)
					: Material("Skybox Material " + path, renderStage, {
							"res/shaders/common/skybox/skybox.vert",
							"res/shaders/common/skybox/skybox.frag"
					}, VK_POLYGON_MODE_FILL) {
				// Create texture
				_texture = std::make_unique<TextureCube>(path, extension);


				// Create descriptor set
				const VkDescriptorImageInfo imageDescriptor = _texture->createDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				render::DescriptorBuilder::begin()
							.bind_image(0, &imageDescriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
						.build(_materialSet.first, _materialSet.second);

				// Create material
				createMaterial();
			}


		private:
			std::unique_ptr<TextureCube> _texture {};
	};
}

