#include "WaterModule.hpp"

void WaterModule::createDescriptors() {
    // Create material descriptor
    std::shared_ptr<Descriptor>& descriptor = _gameObject.createDescriptor();
    _material->setDescriptor(descriptor);

    // Create compute pipeline descriptor
    _computeDescriptor = _gameObject.createDescriptor();
}

std::shared_ptr<Texture2D> _chessboardImage;
void WaterModule::initialize() {
    // Set compute descriptor
    _chessboardImage = std::make_shared<Texture2D>(VkExtent2D{100, 100}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_STORAGE_BIT);
    _computeDescriptor->addSet(2, {
            {0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, _chessboardImage->getView(), _chessboardImage->getSampler(), VK_SHADER_STAGE_COMPUTE_BIT}
    });

    // Initialize compute pipeline
    _pipeline.setDescriptor(_computeDescriptor);
    _pipeline.initialize();


    // Initialize material
    _material->setupDescriptor();
    _material->initialize();
}

void WaterModule::preRender(CommandBuffer &commandBuffer, RenderStage stage) {
    // Run compute shader
    _pipeline.bind(commandBuffer);
    _pipeline.bind(_computeDescriptor);
    vkCmdDispatch(commandBuffer, 1, 1, 1);

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
    _chessboardImage->takeScreenshot("chessboard.png");
}

void WaterModule::render(CommandBuffer &commandBuffer, RenderStage stage) {
    if (stage != _material->getStage()) // Not in the right rendering stage
        return;

    // Bind material to the game object and push camera constants to it
    _material->bind(commandBuffer);
    _material->pushConstants();
    _material->pushDescriptors();

    // Render model
    _model->bind(commandBuffer);
    _model->render();
}

void WaterModule::renderGUI() {
    // TODO
}

void WaterModule::cleanUp() {
    // Clean up model
    _model->cleanUp();

    // Clean up descriptor
    _computeDescriptor->cleanUp();
}
