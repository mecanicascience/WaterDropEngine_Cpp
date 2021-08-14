#pragma once

#include <WaterDropEngine/includes.hpp>

class WaterMaterial : public Material {
	/** Water constant push data  */
	struct PushConstantsData {
		glm::vec3 lightDirection {};
	};


	public:
		explicit WaterMaterial(RenderStage stage, VkPolygonMode polygonMode, uint32_t landMeshBinding, uint32_t landDepthBinding)
				: _landMeshBinding(landMeshBinding), _landDepthBinding(landDepthBinding),
				  Material("Water Material", stage, "WaterApp/waterShader.vert", "WaterApp/waterShader.frag", polygonMode) {
			// Set constants
			_lightDirection = glm::vec3(0.2f, 1.5f, -3.0f);
		}

		/** Initialize the color material */
		void initialize(std::shared_ptr<Descriptor>& descriptor) override {
			// Setup push constants
			_pipeline.addPushConstants(0, sizeof(PushConstantsData));

			// Setup material
			Material::initialize(descriptor);
		}

		void setupDescriptor(std::shared_ptr<Descriptor>& descriptor) override {
			// Set descriptor bindings inputs
			_descriptor->addSet(2, {
				{0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _landMeshBinding, _stage.first},
				{1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _landDepthBinding, _stage.first}
			});
		}

		/** Push constants to the material pipeline */
		void pushConstants() override {
			// Set push constants
			PushConstantsData push {};
			push.lightDirection = glm::normalize(_lightDirection);
			_pipeline.setPushConstants(0, &push);
		}



	private:
		uint32_t _landMeshBinding;
		uint32_t _landDepthBinding;
		glm::vec3 _lightDirection {};
};
