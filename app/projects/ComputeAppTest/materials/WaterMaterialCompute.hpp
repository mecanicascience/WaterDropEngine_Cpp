#pragma once

#include <WaterDropEngine/includes.hpp>

class WaterMaterialCompute : public Material {
	/** Water constant push data  */
	struct PushConstantsData {
		glm::vec3 lightDirection {};
	};


	public:
		explicit WaterMaterialCompute(RenderStage stage, VkPolygonMode polygonMode)
			: Material("Compute Water Material", stage, "ComputeAppTest/drawShaders/drawWaves.vert", "ComputeAppTest/drawShaders/drawWaves.frag", polygonMode) {
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

		std::shared_ptr<Buffer> wavesBuffer {};
		void setWavesBuffer(std::shared_ptr<Buffer>& buffer) {
			wavesBuffer = buffer;
		}

		void setupDescriptor() override {
			// Set descriptor bindings inputs
			_descriptor->addSet(2, {
				{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, wavesBuffer}
			});
		}

		/** Push constants to the material pipeline */
		void pushConstants() override {
			// Set push constants
			PushConstantsData push {};
			push.lightDirection = glm::normalize(_lightDirection);
			_pipeline.setPushConstants(0, &push);
		}

		/*void renderGUI() override {
			wde::gui::GUIRenderer::addVec3Button("Light direction", _lightDirection, 1.0);
		}*/



	private:
		// Push constants
		glm::vec3 _lightDirection {};
};
