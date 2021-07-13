#pragma once

#include "../../src/WaterDropEngine/WdeRenderingEngine/renderer/GraphicsPipeline.hpp"

class CoreAppPipeline : public wde::renderEngine::GraphicsPipeline {
	public:
		CoreAppPipeline(std::string shaderVertLocation, std::string shaderFragLocation) : GraphicsPipeline(shaderVertLocation, shaderFragLocation) {}

		/** @param configInfo The pipeline configuration to modify */
		void setPipelineConfigInfo(PipelineConfigInfo& configInfo) override;

		/** @param configInfo The config info to modify */
		void setGraphicsPipelineLayout(VkPipelineLayoutCreateInfo& configInfo) override;


	private:
		/** A reference to the push constant data */
		VkPushConstantRange pushConstantRange {};
};
