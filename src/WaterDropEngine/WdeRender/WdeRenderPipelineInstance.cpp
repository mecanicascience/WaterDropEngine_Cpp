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
					.bind_buffer(0, &_cameraData->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.bind_buffer(1, &_objectsData->getBufferInfo(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build(_globalSet.first, _globalSet.second);
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

		auto& scene = WaterDropEngine::get().getInstance().getScene();
		// Update global set data
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::updateGlobalSetData");
			// Update camera buffer data
			if (scene.getActiveCamera() == nullptr)
				logger::log(LogLevel::WARN, LogChannel::SCENE) << "No camera in scene." << logger::endl;
			else {
				auto cameraModule = scene.getActiveCamera()->getModule<scene::CameraModule>();

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
				void *data = _objectsData->map();
				auto* objectsData = (scene::GameObject::GPUGameObjectData*) data;
				for (auto& go : scene.getStaticGameObjects())
					objectsData[go->getID()].transformWorldSpace = go->transform->getTransform();
				_objectsData->unmap();
			}

			// Update game objects data
			{
				void *data = _objectsData->map();
				auto* objectsData = (scene::GameObject::GPUGameObjectData*) data;
				for (auto& go : scene.getDynamicGameObjects()) {
					if (go->transform->changed) {
						objectsData[go->getID()].transformWorldSpace = go->transform->getTransform();
						go->transform->changed = false;
					}
				}
				_objectsData->unmap();
			}
		}


		// Create rendering batches
		std::vector<RenderBatch> renderBatches {};
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::createRenderBatches");
			RenderBatch currentBatch {};

			std::shared_ptr<scene::Mesh> lastGOMeshRef = nullptr;
			std::shared_ptr<scene::Material> lastGOMaterialRef = nullptr;

			// Fetch every game objects
			int goActiveID = 0;
			for (auto& go : scene.getGameObjects()) {
				auto meshModule = go->getModule<scene::MeshRendererModule>();

				// If no renderer, or material, or mesh, push last batch
				if (meshModule == nullptr || meshModule->getMaterial() == nullptr || meshModule->getMesh() == nullptr) {
					if (currentBatch.indexCount > 0)
						renderBatches.push_back(currentBatch);

					// No mesh and material
					lastGOMeshRef = nullptr;
					lastGOMaterialRef = nullptr;

					// Empty batch (do not draw this object)
					currentBatch = RenderBatch {};
					goActiveID++;
					continue;
				}

				// If material different from last one, push last batch
				auto& mat = meshModule->getMaterial();
				if (currentBatch.indexCount > 0 && lastGOMaterialRef != mat) {
					if (currentBatch.indexCount > 0)
						renderBatches.push_back(currentBatch);

					// Add this object to a new batch
					currentBatch = RenderBatch {};
					currentBatch.material = mat.get();
					currentBatch.mesh = meshModule->getMesh().get();
					currentBatch.firstIndex = static_cast<int>(goActiveID);
					currentBatch.indexCount = 1;
					goActiveID++;
					continue;
				}
				lastGOMaterialRef = mat;

				// If mesh different from last one, push last batch
				auto& mesh = meshModule->getMesh();
				if (currentBatch.indexCount > 0 && lastGOMeshRef != mesh) {
					if (currentBatch.indexCount > 0)
						renderBatches.push_back(currentBatch);

					// Add this object to a new batch
					currentBatch = RenderBatch {};
					currentBatch.material = mat.get();
					currentBatch.mesh = mesh.get();
					currentBatch.firstIndex = static_cast<int>(goActiveID);
					currentBatch.indexCount = 1;
					goActiveID++;
					continue;
				}
				lastGOMeshRef = mesh;

				// Same material and mesh
				currentBatch.material = mat.get();
				currentBatch.mesh = mesh.get();
				currentBatch.indexCount++;
				if (currentBatch.firstIndex == -1)
					currentBatch.firstIndex = static_cast<int>(goActiveID);
				goActiveID++;
			}

			// Push last batch
			if (currentBatch.indexCount > 0)
				renderBatches.push_back(currentBatch);
		}


		// Engine recording commands to the current frame command buffer
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::render");

			// ==== RENDER COMMANDS ====
			render(commandBuffer, scene, renderBatches);
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
}

