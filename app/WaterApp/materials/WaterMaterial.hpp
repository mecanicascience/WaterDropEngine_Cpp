#pragma once

#include <WaterDropEngine/includes.hpp>

class WaterMaterial : public Material {
	/** Water constant push data  */
	struct PushConstantsData {
		glm::vec3 lightDirection {};
		float alphaMultiplier {};
	};


	public:
	explicit WaterMaterial(RenderStage stage, VkPolygonMode polygonMode, uint32_t landDepthBinding, uint32_t landNormalsBinding, uint32_t landTexture)
				: _landDepthBinding(landDepthBinding), _landNormalsBinding(landNormalsBinding), _landTexture(landTexture),
				  Material("Water Material", stage, "WaterApp/waterShader.vert", "WaterApp/waterShader.frag", polygonMode) {
			// Set constants
			_lightDirection = glm::vec3(0.2f, 1.5f, -3.0f);
		}

		/** Initialize the color material */
		void initialize() override {
			// Setup push constants
			_pipeline.addPushConstants(0, sizeof(PushConstantsData));

			// Setup material
			Material::initialize();
		}

		void setupDescriptor() override {
			// Set descriptor bindings inputs
			_descriptor->addSet(2, {
				{0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _landDepthBinding, _stage.first},
				{1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _landNormalsBinding, _stage.first},
				{2, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _landTexture, _stage.first}
			});
		}

		/** Push constants to the material pipeline */
		void pushConstants() override {
			// Set push constants
			PushConstantsData push {};
			push.lightDirection = glm::normalize(_lightDirection);
			push.alphaMultiplier = _alphaMultiplier;
			_pipeline.setPushConstants(0, &push);
		}

		void renderGUI() override {
			gui::GUIRenderer::addFloatDragger("Alpha Multiplier", _alphaMultiplier, 1.0f, 120.0f, 0.1f);
		}



	private:
		// Water material bindings
		uint32_t _landDepthBinding;
		uint32_t _landNormalsBinding;
		uint32_t _landTexture;

		// Push constants
		glm::vec3 _lightDirection {};
		float _alphaMultiplier {};
};
