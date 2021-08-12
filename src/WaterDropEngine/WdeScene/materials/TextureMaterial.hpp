#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"

namespace wde::scene {
	class TextureMaterial : public Material {
		public:
			/**
			 * Create a new texture material
			 * @param stage Rendering stage of the texture
			 * @param relativeTexturePath The relative path to the texture image (end path will be "res/texture/given_path")
			 */
			explicit TextureMaterial(RenderStage stage, const std::string& relativeTexturePath)
					: Material("Texture Material", stage, "texture/textureShader.vert", "texture/textureShader.frag") {
				// Create texture
				_materialTexture = std::make_unique<Texture2D>("res/textures/" + relativeTexturePath);
			}

			// Set the descriptor
			void setupDescriptor(std::shared_ptr<Descriptor>& descriptor) override {
				// Add material set (binding 2)
				descriptor->addSet(2, {
					{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _materialTexture->getView(), _materialTexture->getSampler()}
				});
			}


		private:
			std::unique_ptr<Texture2D> _materialTexture;
	};
}
