#pragma once

#include "Material.hpp"

namespace wde::scene {
	class ColorMaterial : public Material {
		struct GPUMaterialData {
			// Simple material color
			glm::vec4 color {};
		};

		public:
			explicit ColorMaterial(std::pair<int, int> renderStage, Color materialColor)
			    : Material("Color Material " + materialColor.toString(), renderStage, {
                            "res/shaders/common/color/color.vert.spv",
                            "res/shaders/common/color/color.frag.spv"
                        }) {
				// Create material buffer
				_materialData = std::make_unique<render::Buffer>(sizeof(GPUMaterialData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
				{
					GPUMaterialData matData {};
					matData.color = glm::vec4(materialColor._r, materialColor._g, materialColor._b, 0.0);

					void* data = _materialData->map();
					memcpy(data, &matData, sizeof(GPUMaterialData));
					_materialData->unmap();
				}

				// Create descriptor set
				render::DescriptorBuilder::begin()
							.bind_buffer(0, &_materialData->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
						.build(_materialSet.first, _materialSet.second);

				// Create material
				createMaterial();
			}


		private:
			/** The material descriptor buffer */
			std::unique_ptr<render::Buffer> _materialData;
	};
}

