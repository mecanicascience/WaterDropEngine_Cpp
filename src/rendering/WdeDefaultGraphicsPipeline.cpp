#include "WdeDefaultGraphicsPipeline.hpp"

namespace wde {
    WdeDefaultGraphicsPipeline::~WdeDefaultGraphicsPipeline() {
        // Destroy graphics pipeline
        vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);

        // Destroy pipeline layout
        vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
    }


    void WdeDefaultGraphicsPipeline::initialize(std::string &shaderVertLocation, std::string &shaderFragLocation) {
        createGraphicsPipeline(shaderVertLocation, shaderFragLocation);
    }

    void WdeDefaultGraphicsPipeline::createGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation) {
        createDefaultGraphicsPipeline(shaderVertLocation, shaderFragLocation);
    }



    void WdeDefaultGraphicsPipeline::createDefaultGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation) {
        auto vertShaderCode = Utils::readFile(shaderVertLocation);
        auto fragShaderCode = Utils::readFile(shaderFragLocation);

        // Create temp modules
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        // Create vertex shader infos
        VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // Say at what part of the rendering shader will be used
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main"; // entrypoint

        // Create frag shader infos
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        // Shaders infos list
        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};



        // ==== Choose pipeline rendering configuration ====

        // == Vertex input (select format of data passed to vertex shader and buffers from where to fetch) ==
        VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0; // Don't use buffer (hard coded positions from now in shader)
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0; // Don't use buffer (hard coded positions from now in shader)
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional


        // == Input assembly (groups vertex data into primitives for processing by rest of pipeline) ==
        /* Usual topology values :
         * VK_PRIMITIVE_TOPOLOGY_POINT_LIST : points from vertices
         * VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
         * VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
         * VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
         * VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle */
        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Classic triangles map
        inputAssembly.primitiveRestartEnable = VK_FALSE;


        // == Tesselation (cuts shape in multiple triangles) ==
        // We let it by default (nullptr)
        // VkPipelineTessellationStateCreateInfo tesselationInfo {};
        // tesselationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        // tesselationInfo.patchControlPoints = 32; // Can be at least 32


        // == Viewports and scissors (transform vertices from device coords to window coords) ==
        // We want to render to the window from (0, 0) to (width, height)
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChain.getSwapChainExtent().width;
        viewport.height = (float) swapChain.getSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // The viewport and rendering image zone should be the same
        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = swapChain.getSwapChainExtent();

        // Final struct
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1; // Can use multiple viewports (require GPU feature)
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1; // Can use multiple scissors (require GPU feature)
        viewportState.pScissors = &scissor;


        // == Rasterization (vertices given by vertex shaders turned into fragments to be colored) ==
        // (also performs depth testing (= depth map), face culling (= which polygon face is visible), and scissor test)
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; // Fragments beyond near and far planes are clamped to them as opposed to discard them (use in shadow maps)
        rasterizer.rasterizerDiscardEnable = VK_FALSE;

        /* Determines how fragments are generated for geometry
         * VK_POLYGON_MODE_FILL : fill the area of the polygon with fragments
         * VK_POLYGON_MODE_LINE : polygon edges are drawn as lines (useful for wireframe mode) -> required GPU feature
         * VK_POLYGON_MODE_POINT : polygon vertices are drawn as points -> required GPU feature */
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f; // Thickness of lines in terms of fragments ( != 1f may cause lag)
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Enable culling (don't display) for back faces
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // Vertex order for faces to be considered front-facing

        // Can alter depth values by adding constants, clamping, sloping, ... (disabled here)
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


        // == Multisampling (generate multiple samples for each pixel -> anti-aliasing) ==
        // Disable it for now (require GPU feature)
        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional


        // == Depth and stencil testing ==
        // For now, no depth texture ! (nullptr)
        // VkPipelineDepthStencilStateCreateInfo depthStencil {};


        // == Color blending (combine shader color with color already in framebuffer) ==
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
                  VK_COLOR_COMPONENT_R_BIT
                | VK_COLOR_COMPONENT_G_BIT
                | VK_COLOR_COMPONENT_B_BIT
                | VK_COLOR_COMPONENT_A_BIT;
        // DISABLE BLENDING
        colorBlendAttachment.blendEnable = VK_FALSE; // Do not enable blend between different frames in framebuffer
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
        /* ENABLE BLENDING
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        */
        /* Functioning:
        if blendEnable:
            finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
            finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
        else :
            finalColor = newColor;
        finalColor = finalColor & colorWriteMask; */

        // Used blend factors
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        // =====================================================

        // Create pipeline uniform values
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }


        // ==  Update generated data to the pipeline ==
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        // Shader stages (functionality of the programmable stages of the pipeline)
        pipelineInfo.stageCount = 2; // fragment, then vertex shaders
        pipelineInfo.pStages = shaderStages;

        // Fixed-function state (fixed-functions stages of the pipeline : input assembly, rasterizer, viewport, color blending)
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional

        // Pipeline layout (uniform and push values referenced by the shader and can be updated)
        pipelineInfo.layout = pipelineLayout;

        // Reference the render pass and index of sub pass where pipeline will be used
        pipelineInfo.renderPass = swapChain.getRenderPass();
        pipelineInfo.subpass = 0;

        // If the pipeline derives from a parent one
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        // =====


        // Create the graphic pipeline
        if (vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline.");
        }


        // Destroy temp modules
        vkDestroyShaderModule(device.getDevice(), fragShaderModule, nullptr);
        vkDestroyShaderModule(device.getDevice(), vertShaderModule, nullptr);
    }



    VkShaderModule WdeDefaultGraphicsPipeline::createShaderModule(const std::vector<char>& code) {
        // Create infos
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        // Create Module
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module.");
        }

        return shaderModule;
    }
}
