#include "PipelineCompute.hpp"
#include "../core/CoreInstance.hpp"
#include "../../WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "ShaderUtils.hpp"

namespace wde::renderEngine {
    // Constructors
    PipelineCompute::~PipelineCompute() {
        WDE_PROFILE_FUNCTION();
        // Destroy shader modules
        auto device = CoreInstance::get().getSelectedDevice().getDevice();
        vkDestroyShaderModule(device, _shaderModule, nullptr);

        // Destroy pipeline
        vkDestroyPipeline(device, _pipeline, nullptr);
        vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

        // Destroy command buffer reference
        _commandBuffer = nullptr;
    }

    void PipelineCompute::initialize() {
        WDE_PROFILE_FUNCTION();
        Logger::debug("Initializing compute pipeline.", LoggerChannel::RENDERING_ENGINE);

        // Create the pipeline shader modules
        createShaderModule();

        // Create the pipeline layout
        createPipelineLayout();

        // Create the pipeline
        createPipeline();

        // Set pipeline initialized status
        _initialized = true;

        // Initialize pipeline descriptor
        if (_descriptor != nullptr)
            _descriptor->initialize();
    }



    // Core functions
    void PipelineCompute::createShaderModule() {
        WDE_PROFILE_FUNCTION();

        // Create shader module
        std::vector<char> shaderContent = WdeFileUtils::readFile(_shaderStage);
        auto shaderModule = ShaderUtils::createShaderModule(shaderContent);
        auto shaderStageType = ShaderUtils::getShaderStage(_shaderStage);

        // ShaderUtils infos
        _shaderDescription.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        _shaderDescription.stage = shaderStageType;
        _shaderDescription.module = shaderModule;
        _shaderDescription.pName = "main"; // entrypoint
        _shaderDescription.flags = 0; // Optional
        _shaderDescription.pNext = nullptr; // Optional
        _shaderDescription.pSpecializationInfo = nullptr; // Optional

        // Set shader module
        _shaderModule = shaderModule;
    }

    std::vector<VkDescriptorSetLayout> _descriptorsVec {};
    void PipelineCompute::createPipelineLayout() {
        WDE_PROFILE_FUNCTION();
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;


        // Push constants
        pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(_pushConstantsValues.size());
        if (!_pushConstantsValues.empty())
            pipelineLayoutCreateInfo.pPushConstantRanges = _pushConstantsValues.data();
        else
            pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


        // Descriptor sets
        if (_descriptor == nullptr) {
            pipelineLayoutCreateInfo.setLayoutCount = 0;
            pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        }
        else {
            _descriptorsVec.clear();
            _descriptor->createLayouts();

            std::vector<VkDescriptorSetLayout>& descriptorVector = _descriptor->getLayouts();
            for (auto& v : descriptorVector)
                if (v != VK_NULL_HANDLE)
                    _descriptorsVec.push_back(v);

            pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(_descriptorsVec.size());
            pipelineLayoutCreateInfo.pSetLayouts = _descriptorsVec.data();
        }


        // Create layout
        if (vkCreatePipelineLayout(CoreInstance::get().getSelectedDevice().getDevice(), &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
            throw WdeException("Failed to create pipelines layout.", LoggerChannel::RENDERING_ENGINE);
    }

    void PipelineCompute::createPipeline() {
        WDE_PROFILE_FUNCTION();

        // Create pipeline
        VkComputePipelineCreateInfo pipelineCreateInfo {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stage = _shaderDescription;
        pipelineCreateInfo.layout = _pipelineLayout;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;
        vkCreateComputePipelines(CoreInstance::get().getSelectedDevice().getDevice(), nullptr, 1, &pipelineCreateInfo, nullptr, &_pipeline);
    }
}
