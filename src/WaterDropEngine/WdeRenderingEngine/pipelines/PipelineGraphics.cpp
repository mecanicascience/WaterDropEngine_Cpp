#include "PipelineGraphics.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "../core/CoreInstance.hpp"

namespace wde::renderEngine {
	PipelineGraphics::PipelineGraphics(RenderStage renderStage, std::vector<std::string> shaderStages, VkPrimitiveTopology vertexTopology,
			VkPolygonMode polygonDrawMode, VkCullModeFlags cullingMode, VkFrontFace normalOrientation)
			: _renderStage(std::move(renderStage)), _shaderStages(std::move(shaderStages)), _vertexTopology(vertexTopology),
			  _polygonDrawMode(polygonDrawMode), _cullingMode(cullingMode), _normalOrientation(normalOrientation), _pipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS) {

		// Create the pipeline shader modules
		createShaderModules();

		// Create the pipeline layout
		createPipelineLayout();

		// Create the pipeline attributes
		createPipelineAttributes();

		// Create the pipeline
		createPipeline();
	}

	PipelineGraphics::~PipelineGraphics() {
		// Destroy shader modules
		auto device = CoreInstance::get().getSelectedDevice().getDevice();
		for (const auto &shaderModule : _shaderModules)
			vkDestroyShaderModule(device, shaderModule, nullptr);

		// Destroy pipeline
		vkDestroyPipeline(device, _pipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
	}



	void PipelineGraphics::createShaderModules() {
		for (const std::string &shaderStage : _shaderStages) {
			// Create shader module
			std::vector<char> shaderContent = WdeFileUtils::readFile(shaderStage);
			auto shaderModule = ShaderUtils::createShaderModule(shaderContent);
			auto shaderStageType = ShaderUtils::getShaderStage(shaderStage);

			// ShaderUtils infos
			VkPipelineShaderStageCreateInfo shaderStageInfo {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = shaderStageType;
			shaderStageInfo.module = shaderModule;
			shaderStageInfo.pName = "main"; // entrypoint
			shaderStageInfo.flags = 0; // Optional
			shaderStageInfo.pNext = nullptr; // Optional
			shaderStageInfo.pSpecializationInfo = nullptr; // Optional

			// Push module to the list
			_shaderStagesInfo.emplace_back(shaderStageInfo);
			_shaderModules.emplace_back(shaderModule);
		}
	}

	void PipelineGraphics::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0; // Optional
		pipelineLayoutCreateInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

		// Create layout
		if (vkCreatePipelineLayout(CoreInstance::get().getSelectedDevice().getDevice(), &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
			throw WdeException("Failed to create pipelines layout.", LoggerChannel::RENDERING_ENGINE);
	}


	void PipelineGraphics::createPipelineAttributes() {
		// == Input assembly (groups vertex data into primitives for processing by rest of pipelines) ==
		_configInfo.inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		_configInfo.inputAssemblyState.topology = _vertexTopology;
		_configInfo.inputAssemblyState.primitiveRestartEnable = VK_FALSE;



		// == Rasterization (vertices given by vertex shaders turned into fragments to be colored) ==
		// (also performs depth testing (= depth map), face culling (= which polygon face is visible), and scissor test)
		_configInfo.rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		_configInfo.rasterizationState.depthClampEnable = VK_FALSE; // Fragments beyond near (0) and far planes (1) are clamped to them (0 and 1) as opposed to discard them (use in shadow maps)
		_configInfo.rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		_configInfo.rasterizationState.polygonMode = _polygonDrawMode;
		_configInfo.rasterizationState.lineWidth = 1.0f; // Thickness of lines in terms of fragments ( != 1f may cause lag)
		_configInfo.rasterizationState.cullMode = _cullingMode; // Enable culling (don't display) for back faces
		_configInfo.rasterizationState.frontFace = _normalOrientation; // Vertex order for faces to be considered front-facing

		// Can alter depth values by adding constants, clamping, sloping, ... (disabled here)
		_configInfo.rasterizationState.depthBiasEnable = VK_FALSE;
		_configInfo.rasterizationState.depthBiasConstantFactor = 0.0f; // Optional
		_configInfo.rasterizationState.depthBiasClamp = 0.0f; // Optional
		_configInfo.rasterizationState.depthBiasSlopeFactor = 0.0f; // Optional



		// == Depth and stencil testing ==
		// == Color blending (combine shader color with color already in framebuffer) ==
		_configInfo.blendAttachmentState.colorWriteMask =
				  VK_COLOR_COMPONENT_R_BIT
				| VK_COLOR_COMPONENT_G_BIT
				| VK_COLOR_COMPONENT_B_BIT
				| VK_COLOR_COMPONENT_A_BIT;
		// DISABLE BLENDING
		_configInfo.blendAttachmentState.blendEnable = VK_FALSE; // Do not enable blend between different frames in framebuffer
		/* ENABLE BLENDING
			blendAttachmentState.blendEnable = VK_TRUE;
			blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
			blendAttachmentState.alphaBlendOp = VK_BLEND_OP_MAX;
		*/

		// Used blend factors
		_configInfo.colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		_configInfo.colorBlendState.logicOpEnable = VK_FALSE;
		_configInfo.colorBlendState.logicOp = VK_LOGIC_OP_COPY; // Optional
		_configInfo.colorBlendState.attachmentCount = 1;
		_configInfo.colorBlendState.pAttachments = &_configInfo.blendAttachmentState;
		_configInfo.colorBlendState.blendConstants[0] = 0.0f; // Optional
		_configInfo.colorBlendState.blendConstants[1] = 0.0f; // Optional
		_configInfo.colorBlendState.blendConstants[2] = 0.0f; // Optional
		_configInfo.colorBlendState.blendConstants[3] = 0.0f; // Optional

		// Depth stencil
		_configInfo.depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		_configInfo.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		_configInfo.depthStencilState.front = _configInfo.depthStencilState.back;
		_configInfo.depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
		_configInfo.depthStencilState.depthBoundsTestEnable = VK_FALSE;
		_configInfo.depthStencilState.minDepthBounds = 0.0f;  // Optional
		_configInfo.depthStencilState.maxDepthBounds = 1.0f;  // Optional
		_configInfo.depthStencilState.stencilTestEnable = VK_FALSE;
		_configInfo.depthStencilState.front = {};  // Optional
		_configInfo.depthStencilState.back = {};   // Optional

		// Enable depth read and write
		_configInfo.depthStencilState.depthTestEnable = VK_TRUE;
		_configInfo.depthStencilState.depthWriteEnable = VK_TRUE;
		/*// Disable depth read and write
		configInfo.depthStencilState.depthTestEnable = VK_FALSE;
		configInfo.depthStencilState.depthWriteEnable = VK_FALSE;*/



		// == Viewports and scissors (transform vertices from device coords to window coords) ==
		// Done elsewhere to handle window resize
		// Final struct
		_configInfo.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		_configInfo.viewportState.viewportCount = 1; // Can use multiple viewports (require GPU feature)
		_configInfo.viewportState.pViewports = nullptr;
		_configInfo.viewportState.scissorCount = 1; // Can use multiple scissors (require GPU feature)
		_configInfo.viewportState.pScissors = nullptr;



		// == Tesselation (cuts shape in multiple triangles) ==
		_configInfo.tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		_configInfo.tessellationState.patchControlPoints = 3; // Can be at least 32



		// == Multisampling (generate multiple samples for each pixel -> anti-aliasing) ==
		// Disable it for now (require GPU feature)
		_configInfo.multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		_configInfo.multisampleState.sampleShadingEnable = VK_FALSE;
		_configInfo.multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		_configInfo.multisampleState.minSampleShading = 1.0f; // Optional
		_configInfo.multisampleState.pSampleMask = nullptr; // Optional
		_configInfo.multisampleState.alphaToCoverageEnable = VK_FALSE; // Optional
		_configInfo.multisampleState.alphaToOneEnable = VK_FALSE; // Optional

		// Enable dynamic states (handle resizing)
		_configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		_configInfo.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		_configInfo.dynamicState.pDynamicStates = _configInfo.dynamicStateEnables.data();
		_configInfo.dynamicState.dynamicStateCount = static_cast<uint32_t>(_configInfo.dynamicStateEnables.size());
		_configInfo.dynamicState.flags = 0;
	}


	void PipelineGraphics::createPipeline() {
		// Set vertex inputs
		_configInfo.vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		_configInfo.vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0; // Don't use buffer (hard coded positions from now in shader)
		_configInfo.vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr; // Optional
		_configInfo.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0; // Don't use buffer (hard coded positions from now in shader)
		_configInfo.vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr; // Optional


		// Binds the pipeline infos in the pipeline create struct
		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(_shaderStagesInfo.size());
		pipelineCreateInfo.pStages = _shaderStagesInfo.data();

		// Fixed-function states
		pipelineCreateInfo.pVertexInputState = &_configInfo.vertexInputStateCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &_configInfo.inputAssemblyState;
		pipelineCreateInfo.pTessellationState = &_configInfo.tessellationState;
		pipelineCreateInfo.pViewportState = &_configInfo.viewportState;
		pipelineCreateInfo.pRasterizationState = &_configInfo.rasterizationState;
		pipelineCreateInfo.pMultisampleState = &_configInfo.multisampleState;
		pipelineCreateInfo.pDepthStencilState = &_configInfo.depthStencilState;
		pipelineCreateInfo.pColorBlendState = &_configInfo.colorBlendState;
		pipelineCreateInfo.pDynamicState = &_configInfo.dynamicState;

		// Pipeline layout
		pipelineCreateInfo.layout = _pipelineLayout;

		// Reference the render pass and index of sub pass where pipelines will be used
		pipelineCreateInfo.renderPass = *CoreInstance::get().getRenderer()->getRenderPass(_renderStage.first)->getRenderPass();
		pipelineCreateInfo.subpass = _renderStage.second;

		// If the pipelines derives from a parent one
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineCreateInfo.basePipelineIndex = -1; // Optional


		// Create the graphic pipelines
		if (vkCreateGraphicsPipelines(CoreInstance::get().getSelectedDevice().getDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline) != VK_SUCCESS)
			throw WdeException("Failed to create graphics pipelines.", LoggerChannel::RENDERING_ENGINE);
	}
}