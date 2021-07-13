#include "CoreAppPipeline.hpp"
#include "../CoreApp.hpp"


// Core functions
void CoreAppPipeline::setPipelineConfigInfo(PipelineConfigInfo& configInfo) {
	// Nothing to currently modify from the default pipeline
}


void CoreAppPipeline::setGraphicsPipelineLayout(VkPipelineLayoutCreateInfo& configInfo) {
	// Set that we use our custom push constant data
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // can access data in frag and vert shaders
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(CoreApp::SimplePushConstantData);

	// Updated pipelineLayout infos
	configInfo.pushConstantRangeCount = 1;
	configInfo.pPushConstantRanges = &pushConstantRange;
}

