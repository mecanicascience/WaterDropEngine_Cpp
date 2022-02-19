#include "WdeRenderPipelineInstance.hpp"
#include "../WaterDropEngine.hpp"
#include "../WdeScene/gameObjects/modules/CameraModule.hpp"

namespace wde::render {
	WdeRenderPipelineInstance::WdeRenderPipelineInstance() {
		WDE_PROFILE_FUNCTION();

		// Create camera descriptor set
		_cameraData = std::make_unique<Buffer>(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		DescriptorBuilder::begin()
				.bind_buffer(0, &_cameraData->getBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
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
		// Engine recording commands to the current frame command buffer
		{
			WDE_PROFILE_SCOPE("wde::render::WdeRenderPipelineInstance::tick()::render");
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


			// ==== RENDER COMMANDS ====
			render(commandBuffer, scene);
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

