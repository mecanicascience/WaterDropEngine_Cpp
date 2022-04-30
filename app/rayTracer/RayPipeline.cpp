#include <random>
#include "RayPipeline.hpp"
#include "../../src/WaterDropEngine/WaterDropEngine.hpp"

void RayPipeline::setup() {
	// Create passes attachments
	setAttachments({
		{0, "Depth texture", RenderAttachment::DEPTH},
		{1, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
	});

	// Create passes and subpasses structure
	setStructure({
		{0, {
			{0, {}, { 0, 1 }}, // Scene
			{1, {}, { 1 }} // GUI
		}}
	});

	// Initialize GUI
	gui::WdeGUI::initialize(std::pair<int, int>{0, 1});
}

void RayPipeline::render(CommandBuffer &commandBuffer, scene::WdeSceneInstance &scene) {
	// Recreate raytracing image if needed
	if (!_image || _viewportWidth != _image->getWidth() || _viewportHeight != _image->getHeight()) {
		if (_viewportWidth == 0 && _viewportHeight == 0) {
			_viewportWidth = 10;
			_viewportHeight = 10;
		}

		WaterDropEngine::get().getRender().getInstance().waitForDevicesReady();
		_image = std::make_shared<CImage>(_viewportWidth, _viewportHeight, ImageFormat::RGBA);
		delete[] _imageData;
		_imageData = new uint32_t[_viewportWidth * _viewportHeight];
		renderRayTracing();
	}

	beginRenderPass(0);
		beginRenderSubPass(0);
		endRenderSubPass();

		beginRenderSubPass(1);
			// Render GUI
			gui::WdeGUI::render(commandBuffer);
		endRenderSubPass();
	endRenderPass();
}

void RayPipeline::cleanUp() {
	// Clean up raytracer
	delete[] _imageData;
	_image.reset();
}

void RayPipeline::renderRayTracing() {
	// Do ray tracing
	static std::mt19937 s_RandomEngine {};
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution {};

	for (uint32_t i = 0; i < _viewportWidth * _viewportHeight; i++) {
		_imageData[i] = s_Distribution(s_RandomEngine);
		_imageData[i] |= 0xff000000;
	}

	// Update image data
	_image->setData(_imageData);

}

void RayPipeline::onNotify(const core::Event& event) {
	#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("RenderZone");
			_viewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
			_viewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);
			if (_image)
				ImGui::Image(_image->getDescriptorSet(), { (float)_image->getWidth(), (float)_image->getHeight() });

			ImGui::End();
			ImGui::PopStyleVar();
		}
	#endif
}

