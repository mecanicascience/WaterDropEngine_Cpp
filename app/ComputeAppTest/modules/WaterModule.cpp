#include "WaterModule.hpp"

void WaterModule::createDescriptors() {
    // Create material descriptor
    std::shared_ptr<Descriptor>& descriptor = _gameObject.createDescriptor();
    _material->setDescriptor(descriptor);

    // Create compute pipeline descriptor
    _computeDescriptor = _gameObject.createDescriptor();

    // Create texture pipeline descriptor
    _textureDisplayDescriptor = _gameObject.createDescriptor();
}

void WaterModule::initialize() {
	// Set image
	_chessboardImage = std::make_shared<Texture2D>(VkExtent2D{100, 100},
												   VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // Compute shader
    _computeDescriptor->addSet(2, {
            {0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, _chessboardImage->getView(), _chessboardImage->getSampler(), VK_SHADER_STAGE_COMPUTE_BIT}
    });
    _computePipeline.setDescriptor(_computeDescriptor);
    _computePipeline.initialize();


    // Set texture descriptor
    _textureDisplayDescriptor->addSet(2, {
    	{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _chessboardImage->getView(), _chessboardImage->getSampler()}
    });
    _textureDisplayPipeline.setDescriptor(_textureDisplayDescriptor);
    _textureDisplayPipeline.initialize();


    // Initialize material
    /*_material->setupDescriptor();
    _material->initialize();*/


    // Run compute shader
    CommandBuffer commandBuffer {true};
    _computePipeline.bind(commandBuffer);
    _computePipeline.bind(_computeDescriptor);
    vkCmdDispatch(commandBuffer, 16, 16, 1);
    commandBuffer.end();
    commandBuffer.submit();
    commandBuffer.waitForQueueIdle();

    // Transition image from compute layout to present layout
    _chessboardImage->toLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void WaterModule::preRender(CommandBuffer &commandBuffer, RenderStage stage) {

    // Wait for compute shader to end // TODO
    /*VkImageMemoryBarrier barrier = _chessboardImage.
    vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);*/

    // Save image to file
    //_chessboardImage->takeScreenshot("chessboard.png");
}

void WaterModule::render(CommandBuffer &commandBuffer, RenderStage stage) {
    /*if (stage != _material->getStage()) // Not in the right rendering stage
        return;*/

    // Bind material to the game object and push camera constants to it
    /*_material->bind(commandBuffer);
    _material->pushConstants();
    _material->pushDescriptors();

    // Render model
    _model->bind(commandBuffer);
    _model->render();*/

    // Display texture image
    _textureDisplayPipeline.bind(commandBuffer);
    _textureDisplayPipeline.bind(_textureDisplayDescriptor);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void WaterModule::renderGUI() {
    // TODO
}

void WaterModule::cleanUp() {
    // Clean up model
    // _model->cleanUp();

    // Clean up descriptors
    _computeDescriptor->cleanUp();
    _textureDisplayDescriptor->cleanUp();
}
