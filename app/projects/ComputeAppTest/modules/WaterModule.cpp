#include "WaterModule.hpp"

WaterModule::WaterModule(GameObject &gameObject, std::shared_ptr<Model> model, std::shared_ptr<Material> material)
	: Module(gameObject, "Water Module"),
	  _model(std::move(model)),
	  _material(std::move(material)),
	  _computePipeline("res/shaders/ComputeAppTest/computeShaders/generateWaves.comp.spv") {}

void WaterModule::createDescriptors() {
    // Create material descriptor
    _material->setDescriptor(_gameObject.createDescriptor());

    // Create compute pipeline descriptor
    _computeDescriptor = _gameObject.createDescriptor();
}

void WaterModule::initialize() {
	// Create buffer
	struct BufferData {
		int defaultVal = 0;
	};
	auto wavesData = std::make_shared<Buffer>(sizeof(BufferData), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

    // Compute shader
    _computeDescriptor->addSet(2, {
    	{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, wavesData, VK_SHADER_STAGE_COMPUTE_BIT}
    });
    _computePipeline.setDescriptor(_computeDescriptor);
    _computePipeline.initialize();


    // Initialize material
    _material->setupDescriptor();
    _material->initialize();


    // Run compute shader
    CommandBuffer commandBuffer {true};
    _computePipeline.bind(commandBuffer);
    _computePipeline.bind(_computeDescriptor);
    vkCmdDispatch(commandBuffer, 32, 32, 1);
    commandBuffer.submitIdle();
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

void WaterModule::cleanUp() {
    // Clean up model
    _model->cleanUp();
}
