#include "WaterModule.hpp"


WaterModule::WaterModule(GameObject &gameObject, int planeSizeFactor, std::shared_ptr<Model> model, std::shared_ptr<WaterMaterialCompute> material)
	: Module(gameObject, "Water Module"),
	  _planeSizeFactor(planeSizeFactor),
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
	wavesData = std::make_shared<Buffer>(sizeof(BufferData), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

    // Compute shader
    _computeDescriptor->addSet(2, {
    	{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, wavesData, VK_SHADER_STAGE_COMPUTE_BIT}
    });
    _computeDescriptor->addSet(3, {
    	{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (int) sizeof(ReadBufferData), VK_SHADER_STAGE_COMPUTE_BIT}
    });
    _computePipeline.setDescriptor(_computeDescriptor);
    _computePipeline.initialize();


    // Initialize material
    _material->setWavesBuffer(wavesData);
    _material->setupDescriptor();
    _material->initialize();

    // Display result
    /*auto bfData = (BufferData*) wavesData->map();
    for (int i : bfData->wavesHeight) {
    	wde::Logger::info("Result " + std::to_string(i), wde::LoggerChannel::MAIN);
    }
	wavesData->unmap();*/
}

auto startTime = std::chrono::system_clock::now();
void WaterModule::render(CommandBuffer &commandBuffer, RenderStage stage) {
	// Update time
	auto newTime = std::chrono::system_clock::now();
	auto deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(newTime - startTime).count();
	ReadBufferData newData {};
	newData.time = deltaTime / 1000.0f;
	_computeDescriptor->getSet(3).updateBuffer(0, &newData);

	// Run compute shader
	CommandBuffer commandBufferCompute {true};
	_computePipeline.bind(commandBufferCompute);
	_computePipeline.bind(_computeDescriptor);

	// Use n*n*1 work groups
	vkCmdDispatch(commandBufferCompute, _planeSizeFactor, _planeSizeFactor, 1);
	commandBufferCompute.submitIdle();

	// If material is not in the right rendering stage
    if (stage != _material->getStage())
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
