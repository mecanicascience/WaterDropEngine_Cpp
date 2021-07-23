#pragma once

#include <utility>
#include <algorithm>

#include "../../../wde.hpp"
#include "Pipeline.hpp"
#include "../renderer/descriptors/RenderStage.hpp"
#include "ShaderUtils.hpp"
#include "../structures/Model.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"

namespace wde::renderEngine {
	class PipelineGraphics : public Pipeline {
		public:
			// Depth texture access values
			enum class Depth {
				None = 0,
				Read = 1,
				Write = 2,
				ReadWrite = Read | Write
			};


			// Constructors
			/**
			 * Creates a new graphics pipeline
			 * @param renderStage The associated pipeline rendering stage
			 * @param shaderStages Paths of the pipeline shaders
			 * @param vertexInputs The models vertices
			 * @param depth The allowed depth texture pipeline accesses
			 * @param vertexTopology The pipeline graphics topology (VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST by default, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, ...)
			 * @param polygonDrawMode How the vertices will be bounded from the indices (VK_POLYGON_MODE_FILL by default, VK_POLYGON_MODE_LINE, VK_POLYGON_MODE_POINT)
			 * @param cullingMode The pipeline culling mode (default VK_CULL_MODE_BACK_BIT (culling enabled for faces backing their faces), VK_CULL_MODE_NONE, VK_CULL_MODE_FRONT_BIT)
			 * @param normalOrientation How the normals will be computed from the indices order (default VK_FRONT_FACE_CLOCKWISE, VK_FRONT_FACE_COUNTER_CLOCKWISE)
			 */
			PipelineGraphics(RenderStage renderStage, std::vector<std::string> shaderStages, std::vector<Model::VertexInput> vertexInputs, Depth depth = Depth::ReadWrite,
			                 VkPrimitiveTopology vertexTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkPolygonMode polygonDrawMode = VK_POLYGON_MODE_FILL,
			                 VkCullModeFlags cullingMode = VK_CULL_MODE_BACK_BIT, VkFrontFace normalOrientation = VK_FRONT_FACE_CLOCKWISE);

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


			// Core functions
			/** Create the given shader modules */
			void createShaderModules();
			/** Create the pipeline layout */
			void createPipelineLayout();
			/** Create the pipeline attributes */
			void createPipelineAttributes();
			/** Create the Vulkan pipeline */
			void createPipeline();



		private:
			// Pipeline description parameters
			RenderStage _renderStage;
			std::vector<std::string> _shaderStages;
			std::vector<Model::VertexInput> _vertexInputs;
			Depth _depth;
			VkPrimitiveTopology _vertexTopology;
			VkPolygonMode _polygonDrawMode;
			VkCullModeFlags _cullingMode;
			VkFrontFace _normalOrientation;

			// Vulkan parameters
			VkPipeline _pipeline = VK_NULL_HANDLE;
			VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
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
			PipelineConfigInfo _configInfo {};
	};
}
