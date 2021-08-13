#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"

namespace wde::scene {
	class TextureMaterial : public Material {
		/**
		 * Material constant push data
		 */
		struct PushConstantLightsData {
			glm::vec3 ambientLightVector;
			float ambientValue;
		};

		public:
			/**
			 * Create a new texture material
			 * @param stage Rendering stage of the texture
			 * @param relativeTexturePath The relative path to the texture image (end path will be "res/texture/given_path")
			 * @param textureFilter The optional image filter (default linear filter)
			 * @param textureAdressMode The repeat image mode (default repeat image)
			 */
			explicit TextureMaterial(RenderStage stage, const std::string& relativeTexturePath, VkFilter textureFilter = VK_FILTER_LINEAR,
									 VkSamplerAddressMode textureAdressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT)
					: Material("Texture Material", stage, "texture/textureShader.vert", "texture/textureShader.frag") {
				// Create texture
				_materialTexture = std::make_unique<Texture2D>("res/textures/" + relativeTexturePath, VK_FORMAT_R8G8B8A8_SRGB, textureFilter, textureAdressMode);
			}

			void initialize(std::shared_ptr<Descriptor>& descriptor) override {
				// Setup push constants
				_pipeline.addPushConstants(0, sizeof(PushConstantLightsData));

				// Setup material
				Material::initialize(descriptor);
			}



			void setupDescriptor(std::shared_ptr<Descriptor>& descriptor) override {
				// Add material set (binding 2)
				descriptor->addSet(2, {
					{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _materialTexture->getView(), _materialTexture->getSampler()}
				});
			}


			void pushConstants() override {
				// Set push constants
				PushConstantLightsData push {};
				push.ambientLightVector = glm::normalize(glm::vec3(0.7f, 0.0f, -0.1f));
				push.ambientValue = 0.2f;
				_pipeline.setPushConstants(0, &push);
			}


		private:
			std::unique_ptr<Texture2D> _materialTexture;
	};
}
