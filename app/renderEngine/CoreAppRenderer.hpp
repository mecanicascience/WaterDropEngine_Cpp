#pragma once

#include "../../src/WaterDropEngine/WdeRenderingEngine/renderer/Renderer.hpp"
#include "../../src/WaterDropEngine/WdeRenderingEngine/renderObjects/GameObject.hpp"

#include <ctime>


using namespace wde::renderEngine;

class CoreAppRenderer : public Renderer {
public:
	// Constructors
	CoreAppRenderer() = default;
	~CoreAppRenderer() = default;

protected:
	// Core functions
	void cleanUp(VkDevice &device) override;

	void loadGameObjects(VkDevice &device, VkPhysicalDevice &physicalDevice) override;

	void createRenderPasses(VkCommandBuffer &commandBuffer, VkPipeline &graphicsPipeline, VkPipelineLayout &pipelineLayout, VkRenderPass &renderPass, VkFramebuffer &swapChainFrameBuffer, VkExtent2D &swapChainExtent) override;



private:
	/** Main model */
	std::vector<GameObject> gameObjects;

	void renderGameObjects(VkCommandBuffer &commandBuffer, VkPipelineLayout &pipelineLayout);
};
