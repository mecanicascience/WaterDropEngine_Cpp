#pragma once

#include <utility>
#include <algorithm>

#include "../../../wde.hpp"
#include "Pipeline.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "ShaderUtils.hpp"
#include "../render/RenderPass.hpp"

namespace wde::render {
	class PipelineGraphics : public Pipeline {
		public:
			enum class Mode {
				Polygon, MRT
			};

			enum class Depth {
				None = 0,
				Read = 1,
				Write = 2,
				ReadWrite = Read | Write
			};


			// Constructors
			/**
			 * Creates a new graphics pipeline
			 * @param renderTarget ID of the render pass and subpass
			 * @param shaderStages Paths of the pipeline shaders
			 * @param pipelineMode The mode this pipeline will run in (Polygon (default) or MRT = Multiple render target)
			 * @param depthMode The depth mode
			 * @param vertexTopology The pipeline graphics topology (VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST by default, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, ...)
			 * @param polygonDrawMode How the vertices will be bounded from the indices (VK_POLYGON_MODE_FILL by default, VK_POLYGON_MODE_LINE, VK_POLYGON_MODE_POINT)
			 * @param cullingMode The pipeline culling mode (default VK_CULL_MODE_BACK_BIT (culling enabled for faces backing their faces), VK_CULL_MODE_NONE, VK_CULL_MODE_FRONT_BIT)
			 * @param normalOrientation How the normals will be computed from the indices order (default VK_FRONT_FACE_COUNTER_CLOCKWISE)
			 */
			explicit PipelineGraphics(std::pair<int, int> renderTarget, std::vector<std::string> shaderStages,
			                          Mode pipelineMode = Mode::Polygon, Depth depthMode = PipelineGraphics::Depth::ReadWrite,
			                          VkPrimitiveTopology vertexTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkPolygonMode polygonDrawMode = VK_POLYGON_MODE_FILL,
			                          VkCullModeFlags cullingMode = VK_CULL_MODE_BACK_BIT, VkFrontFace normalOrientation = VK_FRONT_FACE_COUNTER_CLOCKWISE);

			/* More description:
			 * == Usual topology values ==
			 *  VK_PRIMITIVE_TOPOLOGY_POINT_LIST : points from vertices
			 *  VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
			 *  VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
			 *  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
			 *  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle
			 *
			 * == Usual polygon draw modes ==
			 *  VK_POLYGON_MODE_FILL : fill the area of the polygon with fragments
			 *  VK_POLYGON_MODE_LINE : polygon edges are drawn as lines (useful for wireframe mode) -> requires a GPU feature
			 *  VK_POLYGON_MODE_POINT : polygon vertices are drawn as points -> requires a GPU feature
			 */
			~PipelineGraphics() override;


			// Getters and setters
			const VkPipeline &getPipeline() const override { return _pipeline; };
			const VkPipelineBindPoint &getPipelineBindPoint() const override { return _pipelineBindPoint; }



		private:
			// Pipeline description parameters
			std::vector<std::string> _shaderStages;
			Mode _pipelineMode;
			Depth _depthMode;
			VkPrimitiveTopology _vertexTopology;
			VkPolygonMode _polygonDrawMode;
			VkCullModeFlags _cullingMode;
			VkFrontFace _normalOrientation;

			// Vulkan parameters
			/** Type of the pipeline (graphics, compute, ...) */
			VkPipelineBindPoint _pipelineBindPoint;

			// Shader modules
			std::vector<VkShaderModule> _shaderModules;
			std::vector<VkPipelineShaderStageCreateInfo> _shaderStagesInfo;


			// The struct with pipelines config infos
			struct PipelineConfigInfo {
				VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyState {};
				VkPipelineRasterizationStateCreateInfo rasterizationState {};
				VkPipelineColorBlendAttachmentState blendAttachmentState {};
				VkPipelineColorBlendStateCreateInfo colorBlendState {};
				VkPipelineDepthStencilStateCreateInfo depthStencilState {};
				VkPipelineViewportStateCreateInfo viewportState {};
				VkPipelineMultisampleStateCreateInfo multisampleState {};
				VkPipelineDynamicStateCreateInfo dynamicState {};
				VkPipelineTessellationStateCreateInfo tessellationState {};
				std::vector<VkDynamicState> dynamicStateEnables;
			};
			std::array<VkPipelineColorBlendAttachmentState, 1> _blendAttachmentStates;
			PipelineConfigInfo _configInfo {};
		};
}
