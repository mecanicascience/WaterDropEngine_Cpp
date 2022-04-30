#include <random>
#include "RayPipeline.hpp"
#include "../../src/WaterDropEngine/WaterDropEngine.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointer"
void RayPipeline::renderRayTracing() {
	if (_imageData == nullptr)
		return;

	// Do ray tracing
	for (int j = _viewportHeight - 1; j >= 0; j--) {
		for (int i = 0; i < _viewportWidth; i++) {
			// Pixel UV coordinate (from (0, 0) at bottom left to (1, 1) at top right)
			glm::vec2 uv {float(i) / static_cast<float>(_viewportWidth - 1), float(_viewportHeight - 1 - j) / static_cast<float>(_viewportHeight - 1)};

			// Compute color
			Color color {uv.x, uv.y, 0, 1.0f};
			_imageData[i + j * _viewportWidth] = color.toRGBA2B();
		}
	}

	// Update image data
	_image->setData(_imageData);
}


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


#pragma clang diagnostic pop