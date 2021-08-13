#pragma once

#include "../../WdeGUI/GUIRenderer.hpp"
#include "../../../wde.hpp"
#include "Material.hpp"

namespace wde::scene {
	class TextureMaterial : public Material {
		/**
		 * Material constant push data
		 */
		struct PushConstantLightsData {
			glm::vec3 ambientLightVector;
			float ambientLevel;
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
					 : _textureAdressMode(textureAdressMode), _relativeTexturePath(relativeTexturePath), Material("Texture Material", stage, "texture/textureShader.vert", "texture/textureShader.frag") {
				// Create texture
				_materialTexture = std::make_unique<Texture2D>("res/textures/" + _relativeTexturePath, VK_FORMAT_R8G8B8A8_SRGB, textureFilter, textureAdressMode);
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
				push.ambientLightVector = glm::normalize(_lightDirection);
				push.ambientLevel = _ambientLevel;
				_pipeline.setPushConstants(0, &push);
			}


			void renderGUI() override {
				gui::GUIRenderer::addVec3Button("Light direction", _lightDirection, 1.0f, 120.0f);
				gui::GUIRenderer::addFloatDragger("Ambient level", _ambientLevel, 0.0f);
			}


			// Serializers
			json serialize() override {
				return json({
					{"type", _materialName},
					{"stage", {
						{"first", _stage.first},
						{"second", _stage.second}
					}},
					{"texturePath", _relativeTexturePath},
					{"textureFilter", _polygonMode},
					{"textureAdressMode", _textureAdressMode},
					{"data", {
						{"lightDirection", SceneSerializerUtils::asJson(_lightDirection)},
						{"ambientLevel", _ambientLevel}
					}}
				});
			}

			void deserialize(json data) override {
				_lightDirection = SceneSerializerUtils::toVec3(data["data"]["lightDirection"]);
				_ambientLevel   = float(data["data"]["ambientLevel"]);
			}


		private:
			// Data
			std::unique_ptr<Texture2D> _materialTexture;
			glm::vec3 _lightDirection {1.0f, 0.0f, 0.0f};
			float _ambientLevel {0.0f};

			// Core values
			std::string _relativeTexturePath;
			VkSamplerAddressMode _textureAdressMode;
	};
}
