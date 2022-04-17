#include "WdeRenderPipelineInstance.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::render {
	WdeRenderPipelineInstance::WdeRenderPipelineInstance() {
		WDE_PROFILE_FUNCTION();

		// Camera data buffer
		_cameraData = std::make_unique<Buffer>(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		// Objects buffer
		_objectsData = std::make_unique<Buffer>(sizeof(scene::GameObject::GPUGameObjectData) * Config::MAX_SCENE_OBJECTS_COUNT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

		// Create global descriptor set
		DescriptorBuilder::begin()
					.bind_buffer(0, *_cameraData, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.bind_buffer(1, *_objectsData, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(_globalSet.first, _globalSet.second);
	}

	WdeRenderPipelineInstance::~WdeRenderPipelineInstance() {
		WDE_PROFILE_FUNCTION();
		// Destroy render passes
		_passes.clear();
	}


	void WdeRenderPipelineInstance::tick() {
		WDE_PROFILE_FUNCTION();
		render::CoreInstance& renderer = WaterDropEngine::get().getRender().getInstance();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Drawing next frame to the screen with id " << renderer.getCurrentFrame() << "." << logger::endl;

		// Acquire next image from swapchain and signal it to semaphore _imageAvailableSemaphores
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Acquiring swapchain next frame." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::acquireImage");
			renderer.getSwapchain().aquireNextImage();
		}

		// Acquire command buffer
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Acquiring and preparing frame command buffer." << logger::endl;
		render::CommandBuffer& commandBuffer = *renderer.getCommandBuffers()[renderer.getCurrentFrame()];
		if (!commandBuffer.isRunning())
			commandBuffer.begin();

		auto scene = WaterDropEngine::get().getInstance().getScene();
		// Update global set data
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::updateGlobalSetData");
			// Update camera buffer data
			if (scene->getActiveCamera() == nullptr)
				logger::log(LogLevel::WARN, LogChannel::SCENE) << "No camera in scene." << logger::endl;
			else {
				auto cameraModule = scene->getActiveCamera()->getModule<scene::CameraModule>();

				// New data
				GPUCameraData cameraData {};
				cameraData.proj = cameraModule->getProjection();
				cameraData.view = cameraModule->getView();

				// Map data
				void *data = _cameraData->map();
				memcpy(data, &cameraData, sizeof(GPUCameraData));
				_cameraData->unmap();
			}


			// On first update, also update every static game objects
			static bool isFirstTime = true;
			if (isFirstTime) {
				isFirstTime = false;

				// Update static objects // TODO
				void *data = _objectsData->map();
				/*auto* objectsData = (scene::GameObject::GPUGameObjectData*) data;
				for (auto& go : scene->getStaticGameObjects()) {
					objectsData[go->getID()].transformWorldSpace = go->transform->getTransform();

					auto mesh = go->getModule<scene::MeshRendererModule>();
					if (mesh != nullptr && mesh->getMesh() != nullptr)
						objectsData[go->getID()].collisionSphere = mesh->getMesh()->getCollisionSphere();
				}

				// Update dynamic game objects
				for (auto& go : scene->getDynamicGameObjects()) {
					if (go->transform->changed) {
						objectsData[go->getID()].transformWorldSpace = go->transform->getTransform();
						go->transform->changed = false;

						auto mesh = go->getModule<scene::MeshRendererModule>();
						if (mesh != nullptr && mesh->getMesh() != nullptr)
							objectsData[go->getID()].collisionSphere = mesh->getMesh()->getCollisionSphere();
					}
				}*/
				_objectsData->unmap();
			}

			// Update game objects data
			{
				void *data = _objectsData->map();
				/*auto* objectsData = (scene::GameObject::GPUGameObjectData*) data; // TODO
				for (auto& go : scene->getDynamicGameObjects()) {
					if (go->transform->changed) {
						objectsData[go->getID()].transformWorldSpace = go->transform->getTransform();
						go->transform->changed = false;
					}
				}*/
				_objectsData->unmap();
			}
		}


		// Engine recording commands to the current frame command buffer
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::render");
			// ==== RENDER COMMANDS ====
			render(commandBuffer, *scene);
		}

		// Wait for last swapchain image to finish rendering before sending to queue
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Waiting for last swapchain fence to end presentation." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::vkWaitForFences");
			vkWaitForFences(renderer.getDevice().getDevice(), 1, &renderer.getSwapchain().getInFlightFences()[(renderer.getSwapchain().getActiveImageIndex() - 1) % renderer.getMaxFramesInFlight()], VK_TRUE, UINT64_MAX);
		}

		// Submit command buffer
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Submitting command buffer to graphics queue." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::submitCommandBuffer");
			commandBuffer.end();
			commandBuffer.submit(
					renderer.getSwapchain().getInFlightFences()[renderer.getSwapchain().getActiveImageIndex()], // When submitting is done, signal it to the fence
					renderer.getSwapchain().getImageAvailableSemaphores()[renderer.getSwapchain().getActiveImageIndex()], // Wait for swapchain to acquire image
					renderer.getSwapchain().getRenderFinishedSemaphores()[renderer.getSwapchain().getActiveImageIndex()]); // Say that image has been presented
		}

		// Send the current swapchain image to the presentation device queue
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Sending swapchain frame to presentation queue." << logger::endl;
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::present");
			auto presentResult = renderer.getSwapchain().presentToQueue(renderer.getDevice().getPresentQueue());
			if (presentResult != VK_SUCCESS)
				throw WdeException(wde::LogChannel::RENDER, "Failed to present swap chain image.");
		}

		// Increase rendered frame ID
		renderer.getCurrentFrame() = (renderer.getCurrentFrame() + 1) % renderer.getMaxFramesInFlight();
	}

	void WdeRenderPipelineInstance::onWindowResized() {
		WDE_PROFILE_FUNCTION();
		// Recreate render passes
		_passes.clear();
		setStructure(_structure);
	}

	void WdeRenderPipelineInstance::bind(CommandBuffer &commandBuffer, resource::Material *material) const {
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		                        material->getPipeline().getLayout(), 0, 1, &_globalSet.first, 0, nullptr);
	}



	// Pass command manager
	void WdeRenderPipelineInstance::setStructure(const std::vector<RenderPassStructure> &structure) {
		WDE_PROFILE_FUNCTION();
		_structure = structure;

		// Check if attachments setup
		if (_attachments.empty())
			throw WdeException(LogChannel::RENDER, "Tried to create render passes before creating attachments in the render pipeline.");

		// Create passes
		uint32_t iterator = 0;
		for (auto& str : structure) {
			// Check if passes IDs are in order
			if (iterator != str._passID)
				throw WdeException(LogChannel::RENDER, "Missing render pass with ID = " + std::to_string(iterator) + ".");

			// Check if subpasses IDs are in order
			uint32_t iterator2 = 0;
			for (auto& sub : str._subPasses) {
				if (iterator2 != sub._subpassID)
					throw WdeException(LogChannel::RENDER,
					                   "Missing render subpass with ID = " + std::to_string(iterator2) +
					                   " in render pass with ID = " + std::to_string(iterator) + ".");
				iterator2++;
			}

			// Create subpass
			_passes.push_back(std::make_unique<RenderPass>(_attachments, str._subPasses));
			iterator++;
		}
	}


	// Render passes commands
	void WdeRenderPipelineInstance::beginRenderPass(uint32_t index) {
		WDE_PROFILE_FUNCTION();
		if (_currentRenderPassID != -1)
			throw WdeException(LogChannel::RENDER, "Trying to begin pass " + std::to_string(index) + " while pass " + std::to_string(_currentRenderPassID) + " has already began.");
		if (index >= _passes.size())
			throw WdeException(LogChannel::RENDER, "Trying to begin pass " + std::to_string(index) + " which wasn't created.");

		_currentRenderPassID = index;
		_passes[_currentRenderPassID]->start();
	}

	void WdeRenderPipelineInstance::endRenderPass()  {
		WDE_PROFILE_FUNCTION();
		if (_currentRenderSubPassID != -1)
			throw WdeException(LogChannel::RENDER, "Trying to end render pass " + std::to_string(_currentRenderPassID) + " while subpass " + std::to_string(_currentRenderSubPassID) + " has already began.");

		_passes[_currentRenderPassID]->end();
		_currentRenderPassID = -1;
	}

	void WdeRenderPipelineInstance::beginRenderSubPass(uint32_t index) {
		WDE_PROFILE_FUNCTION();
		if (_currentRenderPassID == -1)
			throw WdeException(LogChannel::RENDER, "Trying to begin subpass " + std::to_string(index) + " outside of a render pass.");
		if (_currentRenderSubPassID != -1)
			throw WdeException(LogChannel::RENDER, "Trying to begin subpass " + std::to_string(index) + " while subpass " + std::to_string(_currentRenderSubPassID)
			                                       + " has already began in render pass " + std::to_string(_currentRenderPassID));
		if (index >= _passes[_currentRenderPassID]->getSubPassesCount())
			throw WdeException(LogChannel::RENDER, "Trying to begin pass " + std::to_string(index) + " which wasn't created.");

		_currentRenderSubPassID = index;
		_passes[_currentRenderPassID]->startSubPass(index);
	}

	void WdeRenderPipelineInstance::endRenderSubPass() {
		WDE_PROFILE_FUNCTION();
		if (_currentRenderPassID == -1)
			throw WdeException(LogChannel::RENDER, "Trying to end subpass " + std::to_string(_currentRenderSubPassID) + " outside of a render pass.");

		_passes[_currentRenderPassID]->endSubPass(_currentRenderSubPassID);
		_currentRenderSubPassID = -1;
	}
}

