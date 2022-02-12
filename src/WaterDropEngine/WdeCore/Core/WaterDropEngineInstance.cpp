#include "WaterDropEngineInstance.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde {
	WaterDropEngineInstance::WaterDropEngineInstance() {
		// Create the engine instance
		WaterDropEngine::get();
	}

	void WaterDropEngineInstance::start() {
		// Start the engine
		WaterDropEngine::get().start(*this);
	}

	void WaterDropEngineInstance::renderWDE() {
		render::CoreInstance& renderer = WaterDropEngine::get().getRender().getInstance();
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Drawing next frame to the screen with id " << renderer.getCurrentFrame() << "." << logger::endl;

		// Acquire next image from swapchain and signal it to semaphore _imageAvailableSemaphores
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Acquiring swapchain next frame." << logger::endl;
		renderer.getSwapchain().aquireNextImage();

		// Acquire command buffer
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Acquiring and preparing frame command buffer." << logger::endl;
		render::CommandBuffer& commandBuffer = *renderer.getCommandBuffers()[renderer.getCurrentFrame()];
		if (!commandBuffer.isRunning())
			commandBuffer.begin();

		// Engine rendering
		render(commandBuffer);

		// Wait for last swapchain image to finish rendering before sending to queue
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Waiting for last swapchain fence to end presentation." << logger::endl;
		vkWaitForFences(renderer.getDevice().getDevice(), 1, &renderer.getSwapchain().getInFlightFences()[(renderer.getSwapchain().getActiveImageIndex() - 1) % renderer.getMaxFramesInFlight()], VK_TRUE, UINT64_MAX);

		// Submit command buffer
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Submitting command buffer to graphics queue." << logger::endl;
		commandBuffer.end();
		commandBuffer.submit(renderer.getSwapchain().getInFlightFences()[renderer.getSwapchain().getActiveImageIndex()], // When submitting is done, signal it to the fence
		                      renderer.getSwapchain().getImageAvailableSemaphores()[renderer.getSwapchain().getActiveImageIndex()], // Wait for swapchain to acquire image
		                      renderer.getSwapchain().getRenderFinishedSemaphores()[renderer.getSwapchain().getActiveImageIndex()]); // Say that image has been presented

		// Send the current swapchain image to the presentation device queue
		logger::log(LogLevel::DEBUG, LogChannel::RENDER) << "Sending swapchain frame to presentation queue." << logger::endl;
		auto presentResult = renderer.getSwapchain().presentToQueue(renderer.getDevice().getPresentQueue());
		if (presentResult != VK_SUCCESS)
			throw WdeException(wde::LogChannel::RENDER, "Failed to present swap chain image.");

		// Increase rendered frame ID
		renderer.getCurrentFrame() = (renderer.getCurrentFrame() + 1) % renderer.getMaxFramesInFlight();
	}
}
