#include "GraphicsPipeline.hpp"

namespace wde::renderEngine {
	void GraphicsPipeline::cleanUp() {
		// DeReference pointers
		renderer = nullptr;
	}

	void GraphicsPipeline::cleanUpPipeline(VkDevice &device) {
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderer->getRenderPass(), nullptr);
	}



	void GraphicsPipeline::createGraphicsPipeline(VkDevice &device, VkSwapchainKHR &swapChain, VkExtent2D &swapChainExtent, VkRenderPass &renderPass) {
		// Get pipelines infos
		PipelineConfigInfo configInfo {};
		setDefaultPipelineConfigInfo(configInfo);
		setPipelineConfigInfo(configInfo);

		// Read shaders
		auto vertShaderCode = WdeFileUtils::readFile(shaderVertLocation);
		auto fragShaderCode = WdeFileUtils::readFile(shaderFragLocation);

		// Create temp modules
		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, device);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, device);

		// Create vertex shader infos
		VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // Say at what part of the renderer shader will be used
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main"; // entrypoint
		vertShaderStageInfo.flags = 0;
		vertShaderStageInfo.pNext = nullptr;
		vertShaderStageInfo.pSpecializationInfo = nullptr;


		// Create frag shader infos
		VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.flags = 0;
		fragShaderStageInfo.pNext = nullptr;
		fragShaderStageInfo.pSpecializationInfo = nullptr;

		// Shaders infos list
		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		// == Vertex input (select format of data passed to vertex shader and buffers from where to fetch) ==
		auto bindingDescriptions = Model::Vertex::getBindingDescriptions();
		auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();
		configInfo.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		// Vertex buffers
		configInfo.vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		configInfo.vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		configInfo.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		configInfo.vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		// Set pipelines config infos
		// Create pipelines uniform values
		VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
		setGraphicsPipelineLayout(pipelineLayoutInfo);

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw WdeException("Failed to create pipelines layout.", LoggerChannel::RENDERING_ENGINE);
		}


		// ==  Update generated data to the pipelines ==
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		// Shader stages (functionality of the programmable stages of the pipelines)
		pipelineInfo.stageCount = 2; // fragment, then vertex shaders
		pipelineInfo.pStages = shaderStages;

		// Fixed-function state (fixed-functions stages of the pipelines : input assembly, rasterizer, viewport, color blending)
		pipelineInfo.pVertexInputState = &configInfo.vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

		// Pipeline layout (uniform and push values referenced by the shader and can be updated)
		pipelineInfo.layout = pipelineLayout;

		// Reference the render pass and index of sub pass where pipelines will be used
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;

		// If the pipelines derives from a parent one
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional



		// Create the graphic pipelines
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw WdeException("Failed to create graphics pipelines.", LoggerChannel::RENDERING_ENGINE);
		}


		// Destroy temp modules
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);

		// Say that pipelines created
		pipelineDestroyed = false;
	}

	void GraphicsPipeline::setDefaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {
		// == Input assembly (groups vertex data into primitives for processing by rest of pipelines) ==
		/* Usual topology values :
		 * VK_PRIMITIVE_TOPOLOGY_POINT_LIST : points from vertices
		 * VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
		 * VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
		 * VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
		 * VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle */
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Classic triangles map
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;


		// == Tesselation (cuts shape in multiple triangles) ==
		// We let it by default (nullptr)
		// VkPipelineTessellationStateCreateInfo tesselationInfo {};
		// tesselationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		// tesselationInfo.patchControlPoints = 32; // Can be at least 32


		// == Viewports and scissors (transform vertices from device coords to window coords) ==
		// Done elsewhere to handle window resize
		// Final struct
		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1; // Can use multiple viewports (require GPU feature)
		configInfo.viewportInfo.pViewports = nullptr;
		configInfo.viewportInfo.scissorCount = 1; // Can use multiple scissors (require GPU feature)
		configInfo.viewportInfo.pScissors = nullptr;


		// == Rasterization (vertices given by vertex shaders turned into fragments to be colored) ==
		// (also performs depth testing (= depth map), face culling (= which polygon face is visible), and scissor test)
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE; // Fragments beyond near (0) and far planes (1) are clamped to them (0 and 1) as opposed to discard them (use in shadow maps)
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;

		/* Determines how fragments are generated for geometry
		 * VK_POLYGON_MODE_FILL : fill the area of the polygon with fragments
		 * VK_POLYGON_MODE_LINE : polygon edges are drawn as lines (useful for wireframe mode) -> required GPU feature
		 * VK_POLYGON_MODE_POINT : polygon vertices are drawn as points -> required GPU feature */
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f; // Thickness of lines in terms of fragments ( != 1f may cause lag)
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT; // Enable culling (don't display) for back faces
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // Vertex order for faces to be considered front-facing

		// Can alter depth values by adding constants, clamping, sloping, ... (disabled here)
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f; // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional


		// == Multisampling (generate multiple samples for each pixel -> anti-aliasing) ==
		// Disable it for now (require GPU feature)
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f; // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr; // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional


		// == Depth and stencil testing ==
		// == Color blending (combine shader color with color already in framebuffer) ==
		configInfo.colorBlendAttachment.colorWriteMask =
				VK_COLOR_COMPONENT_R_BIT
				| VK_COLOR_COMPONENT_G_BIT
				| VK_COLOR_COMPONENT_B_BIT
				| VK_COLOR_COMPONENT_A_BIT;
		// DISABLE BLENDING
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE; // Do not enable blend between different frames in framebuffer
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		/* ENABLE BLENDING
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		*/
		/* How it works:
		if blendEnable:
			finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
			finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
		else :
			finalColor = newColor;
		finalColor = finalColor & colorWriteMask; */

		// Used blend factors
		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

		// Depth stencil
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};   // Optional

		// Enable dynamic states (handle resizment)
		configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
		configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
		configInfo.dynamicStateInfo.flags = 0;
	}





	VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code, VkDevice &device) {
		// Create infos
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		// Create Module
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw WdeException("Failed to create shader module.", LoggerChannel::RENDERING_ENGINE);
		}

		return shaderModule;
	}

	void GraphicsPipeline::drawFrame(CoreWindow &window, VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface, SwapChain &swapChain, VkQueue &graphicsQueue, VkQueue &presentQueue, std::vector<VkFramebuffer> &swapChainFrameBuffers, VkExtent2D &swapChainExtent) {
		renderer->drawFrame(window, device, physicalDevice, surface, swapChain, graphicsQueue, presentQueue, graphicsPipeline, pipelineLayout, swapChainFrameBuffers, swapChainExtent);
	}
}
