#include "PipelineGraphics.hpp"

#include <utility>
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	PipelineGraphics::PipelineGraphics(std::pair<int, int> renderTarget, std::vector<std::string> shaderStages, std::vector<resource::VertexInput> vertexInputs, Mode pipelineMode, Depth depthMode, VkPrimitiveTopology vertexTopology,
									   VkPolygonMode polygonDrawMode, VkCullModeFlags cullingMode, VkFrontFace normalOrientation) :
			_shaderStages(std::move(shaderStages)), _vertexTopology(vertexTopology), _vertexInputs(std::move(vertexInputs)), _pipelineMode(pipelineMode), _depthMode(depthMode),
			_polygonDrawMode(polygonDrawMode), _cullingMode(cullingMode), _renderTarget(std::move(renderTarget)),
			_normalOrientation(normalOrientation == VK_FRONT_FACE_CLOCKWISE ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE), // invert vertex orientation
			_pipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Creating graphics pipeline." << logger::endl;

		// Sorts the pipeline vertices
		{
			std::sort(_vertexInputs.begin(), _vertexInputs.end());
		}

		// Create the pipeline shader modules
		{
			for (const std::string &shaderStage : _shaderStages) {
				// Load shader
				auto shader = WaterDropEngine::get().getResourceManager().load<resource::Shader>(shaderStage);

				// Push module to the list
				_shaderStagesInfo.emplace_back(shader->getShaderStageCreateInfo());
				_shaderModules.emplace_back(shader->getShaderModule());
			}
		}
	}

	PipelineGraphics::~PipelineGraphics() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Cleaning up graphics pipeline." << logger::endl;

		// Destroy pipeline
		auto device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();
		vkDestroyPipeline(device, _pipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

		// Destroy command buffer reference
		_commandBuffer = nullptr;
	}


	void PipelineGraphics::initialize() {
		// Create the pipeline layout
		std::vector<VkDescriptorSetLayout> descriptorVec {};
		{
			WDE_PROFILE_SCOPE("wde::render::PipelineGraphics::initialize::createPipelineLayout()");
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;


			// Descriptors sets
			if (_descriptorSetLayoutsList.empty()) {
				pipelineLayoutCreateInfo.setLayoutCount = 0;
				pipelineLayoutCreateInfo.pSetLayouts = nullptr;
			}
			else {
				pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(_descriptorSetLayoutsList.size());
				pipelineLayoutCreateInfo.pSetLayouts = _descriptorSetLayoutsList.data();
			}


			// Push constants
			pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(_pushConstantsValues.size());
			if (!_pushConstantsValues.empty())
				pipelineLayoutCreateInfo.pPushConstantRanges = _pushConstantsValues.data();
			else
				pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


			// Create layout
			if (vkCreatePipelineLayout(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create pipelines layout.");
		}

		// Create the pipeline attributes
		{
			WDE_PROFILE_SCOPE("wde::render::PipelineGraphics::initialize::createLayout()");
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
			// ENABLE BLENDING
			/*_configInfo.blendAttachmentState.blendEnable = VK_TRUE;
			_configInfo.blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			_configInfo.blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			_configInfo.blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			_configInfo.blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			_configInfo.blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
			_configInfo.blendAttachmentState.alphaBlendOp = VK_BLEND_OP_MAX;*/

			// Set pipeline color render target
			_blendAttachmentStates[0].blendEnable = VK_TRUE;
			_blendAttachmentStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			_blendAttachmentStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			_blendAttachmentStates[0].colorBlendOp = VK_BLEND_OP_ADD;
			_blendAttachmentStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			_blendAttachmentStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
			_blendAttachmentStates[0].alphaBlendOp = VK_BLEND_OP_MAX;
			_blendAttachmentStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			// Set pipeline color render target
			_configInfo.colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			_configInfo.colorBlendState.logicOpEnable = VK_FALSE;
			_configInfo.colorBlendState.logicOp = VK_LOGIC_OP_COPY; // Optional
			/* Done just before pipeline layout creation to avoid pointer problems in debugger mode (not sure why)
			 * _configInfo.colorBlendState.attachmentCount = static_cast<uint32_t>(_blendAttachmentStates.size());
			 * _configInfo.colorBlendState.pAttachments = _blendAttachmentStates.data(); */
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

			// Enable or disable depth image read and write
			switch (_depthMode) {
				case Depth::None:
					_configInfo.depthStencilState.depthTestEnable = VK_FALSE;
					_configInfo.depthStencilState.depthWriteEnable = VK_FALSE;
					break;
				case Depth::Read:
					_configInfo.depthStencilState.depthTestEnable = VK_TRUE;
					_configInfo.depthStencilState.depthWriteEnable = VK_FALSE;
					break;
				case Depth::Write:
					_configInfo.depthStencilState.depthTestEnable = VK_FALSE;
					_configInfo.depthStencilState.depthWriteEnable = VK_TRUE;
					break;
				case Depth::ReadWrite:
					_configInfo.depthStencilState.depthTestEnable = VK_TRUE;
					_configInfo.depthStencilState.depthWriteEnable = VK_TRUE;
					break;
			}



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

		// Create MRT pipeline
		if (_pipelineMode == Mode::MRT) {
			WDE_PROFILE_SCOPE("wde::render::PipelineGraphics::initialize::createPipelineMRT()");
			// Blend attachment states
			auto attachmentCount = WaterDropEngine::get().getInstance().getPipeline().getRenderPass(_renderTarget.first).getAttachmentColorCount(_renderTarget.second);
			std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates;
			blendAttachmentStates.reserve(attachmentCount);

			for (uint32_t i = 0; i < attachmentCount; i++) {
				VkPipelineColorBlendAttachmentState blendAttachmentState = {};
				blendAttachmentState.blendEnable = VK_TRUE;
				blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
				blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
				blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				blendAttachmentStates.emplace_back(blendAttachmentState);
			}

			_configInfo.colorBlendState.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
			_configInfo.colorBlendState.pAttachments = blendAttachmentStates.data();
		}
		// Else normal mode (polygon mode)

		// Create the pipeline
		{
			WDE_PROFILE_SCOPE("wde::render::PipelineGraphics::initialize::createPipeline()");
			// Get vertices and indices bindings
			std::vector<VkVertexInputBindingDescription> bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
			uint32_t lastAttribute = 0;

			for (const auto &vertexInput : _vertexInputs) {
				for (const auto &binding : vertexInput.getBindingDescriptions())
					bindingDescriptions.emplace_back(binding);

				for (const auto &attribute : vertexInput.getAttributeDescriptions()) {
					auto &newAttribute = attributeDescriptions.emplace_back(attribute);
					newAttribute.location += lastAttribute;
				}

				if (!vertexInput.getAttributeDescriptions().empty())
					lastAttribute = attributeDescriptions.back().location + 1;
			}


			// Set vertex inputs
			_configInfo.vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			if (_vertexInputs.empty()) { // Assume hard coded shader positions
				_configInfo.vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
				_configInfo.vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
				_configInfo.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
				_configInfo.vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;
			}
			else { // Assume positions coded in buffer
				_configInfo.vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
				_configInfo.vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();
				_configInfo.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				_configInfo.vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			}


			// Binds the pipeline infos in the pipeline create struct
			if (_shaderStagesInfo.empty())
				throw WdeException(LogChannel::RENDER, "Tried to create a graphics pipeline without any shader.");

			VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.stageCount = static_cast<uint32_t>(_shaderStagesInfo.size());
			pipelineCreateInfo.pStages = _shaderStagesInfo.data();

			// Set color attachments
			_configInfo.colorBlendState.attachmentCount = static_cast<uint32_t>(_blendAttachmentStates.size());
			_configInfo.colorBlendState.pAttachments = _blendAttachmentStates.data();

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
			pipelineCreateInfo.renderPass = WaterDropEngine::get().getInstance().getPipeline().getRenderPass(_renderTarget.first).getRenderPass();
			pipelineCreateInfo.subpass = _renderTarget.second;

			// If the pipelines derives from a parent one
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineCreateInfo.basePipelineIndex = -1; // Optional


			// Create the graphic pipelines
			if (vkCreateGraphicsPipelines(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline) != VK_SUCCESS)
				throw WdeException(LogChannel::RENDER, "Failed to create graphics pipelines.");
		}

		// Release shader modules
		for (const std::string &shaderStage : _shaderStages)
			WaterDropEngine::get().getResourceManager().release(shaderStage);

		// Set pipeline initialized status
		_initialized = true;
	}
}
