#pragma once

#include "../../../wde.hpp"
#include "Material.hpp"

namespace wde::scene {
	class ColorMaterial : public Material {
		/**
		 * Material simple constant push data
		 */
		struct PushConstantLightsData {
			glm::vec3 ambientLightVector;
			float ambientValue;
		};


		public:
			explicit ColorMaterial(RenderStage stage)
				: Material("Color Material", stage, "color/colorShader.vert", "color/colorShader.frag") {}

			/** Initialize the color material */
			void initialize(std::shared_ptr<Descriptor>& descriptor) override {
				// Setup push constants
				_pipeline.addPushConstants(0, sizeof(PushConstantLightsData));

				// Setup material
				Material::initialize(descriptor);
			}

			/** Push constants to the material pipeline */
			void pushConstants() override {
				// Set push constants
				PushConstantLightsData push {};
				push.ambientLightVector = glm::normalize(glm::vec3(0.7f, 0.0f, -0.1f));
				push.ambientValue = 0.2f;
				_pipeline.setPushConstants(0, &push);
			}
	};
}

